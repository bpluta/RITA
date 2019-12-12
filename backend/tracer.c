#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ptrace.h>
#include <unistd.h>

#include <sys/wait.h>
#include <sys/user.h>

#include <mach/mach.h>

#include "decoder.c"
#include "encoder.c"

#define LOGFILE_PATH "./traces/trace"

void get_threads(mach_port_t *task, thread_act_array_t *threads) {
   mach_msg_type_number_t threadCount;

   kern_return_t err = task_threads(*task, threads, &threadCount);
   if (err != KERN_SUCCESS) {
       printf("tash_threads failed\n");
   }
}

void get_state(thread_act_t *thread, x86_thread_state_t *state) {
    mach_msg_type_number_t count = x86_THREAD_STATE_COUNT;

    kern_return_t err = thread_get_state(*thread, x86_THREAD_STATE, (thread_state_t)state, &count);
    if (err != KERN_SUCCESS) {
        printf("thread_get_state failed\n");
    }
}

void print_state(x86_thread_state_t *state, int counter) {
    printf("%#016llx -- %d\n", state->uts.ts64.__rip, counter);
}

void dump_mem(unsigned long long rip, char *buf) {
    printf("%#016llx :: ", rip);
    for (int i=0; i<16; i++) {
        printf("%02hhx ", *(buf+i));
    }
    printf("\n");
}

static int flag = 0;

void get_instruction(x86_thread_state_t *state, mach_port_t task, RegisterBuffer *registers, int counter) {
    unsigned long long rip = state->uts.ts64.__rip;
    uintptr_t address = 0;

    mach_msg_type_number_t num = 15;
    kern_return_t err = vm_read(task, rip, 15, &address, &num);
    if (err != KERN_SUCCESS) {
        printf("mach_vm_read failed\n");
        return;
    }
    int buffer_size = 256;
    char buffer[buffer_size];
    decode(rip,((unsigned long long *)(address)), &buffer, buffer_size, state, registers);
    // if (flag == 0) {
    //     flag = 1;
    // } else if (flag == 1) {

    log_instruction(counter, (void *)(address));//&buffer);
    //     flag = 2;
    // }

    // dump_mem(rip,((unsigned long long *)(address)));
}


void fprint_wait_status(FILE *stream, int status)
{
    if( WIFSTOPPED(status) ) {
        fprintf(stream, "Child stopped: %d\n", WSTOPSIG(status));
    }
    if( WIFEXITED(status) ) {
        fprintf(stream, "Child exited: %d\n", WEXITSTATUS(status));
    }
    if( WIFSIGNALED(status) ) {
        fprintf(stream, "Child signaled: %d\n", WTERMSIG(status));
    }
    if( WCOREDUMP(status) ) {
        fprintf(stream, "Core dumped.\n");
    }
}

int ptrace_instruction_pointer(int pid, uint64_t *rip) {
    printf("Executing some instruction %llx\n", rip);
    return 0;
}

void get_saved_registers(RegisterBuffer *registers) {
    if (!registers->count) { return; }
    printf("\t::");
    for (int i=0; i<registers->count; i++) {
        get_register_value(registers->data[i]);
    }
    printf("\n");
    registers->clear(registers);
}

int singlestep(int pid) {
    int retval, status;
    printf("%s\n", "Right before step");
    retval = ptrace(PT_STEP, pid, 0, 0);
    if( retval == 0 ) {
        printf("retval\n");
        return retval;
    }
    waitpid(pid, &status, 0);
    return status;
}

int main(int argc, char **argv, char **envp) {
	int target_pid;
	long ret;
    uint64_t rip;
    int status;
    // decode();
    init();

    if (argc < 2) {
        fprintf(stderr, "Usage: %s elffile arg0 arg1 ...\n", argv[0]);
        exit(-1);
    }
    char *program = argv[1];
    char ** child_args = (char**) &argv[1];

    pid_t pid = fork();
    if (pid == -1) {
        fprintf(stderr, "Error forking: %s\n", strerror(errno));
        exit(-1);
    }
    if (pid == 0) {
        /* Child */
        if (ptrace(PT_TRACE_ME,0,0,0)) {
            fprintf(stderr, "Error setting TRACEME: %s\n", strerror(errno));
            exit(-1);
        }
        execve(program,child_args,envp);
    }
    else {
        /* Parent */
        int status = 0;
        mach_port_t task;
        RegisterBuffer registers;

        pid_t retpid = waitpid(pid, &status, WUNTRACED);
        kern_return_t err = task_for_pid (mach_task_self (), (pid_t)retpid, &task);
        if (err != KERN_SUCCESS) {
            printf("task_for_pid failure\n");
            return 0;
        }

        thread_act_array_t threads = NULL;
        get_threads(&task, &threads);
        thread_act_t current_thread = (thread_act_t)threads[0];

        x86_thread_state_t state;
        get_state(&current_thread, &state);

        int counter = 1;
        print_state(&state, counter);
        registers = registerBufferInit();
        log_init(LOGFILE_PATH);

        while (WIFSTOPPED(status)) {
            counter++;
            if (ptrace(PT_STEP, pid, (caddr_t)1, 0)) {
                printf("%s\n", "Ptrace error");
            }
            waitpid(pid, &status, WUNTRACED);

            get_state(&current_thread, &state);
            get_saved_registers(&registers);
            get_instruction(&state, task, &registers, counter); //print_state(&state, counter);
        }
        log_close();
        free(registers.data);
        puts("\tdetach");
        fprintf(stderr, "Detaching\n");
        ptrace(PT_DETACH, pid, 0, 0);
    }
    return 0;
}
