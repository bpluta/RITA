#ifndef MSG_H
#define MSG_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ptrace.h>

#include <mach/mach.h>
#include <assert.h>
#include <mach/mach_types.h>
#include <mach-o/dyld_images.h>
#include <mach/exception_types.h>
#include <stdbool.h>

#include <mach/i386/thread_status.h>
#include <sys/ucontext.h>
#include <mach/i386/_structs.h>

#define DEBUG 0

#if defined(DEBUG) && DEBUG != 0
 #define DEBUG_PRINT(fmt, args...) printf(fmt, ##args)
#else
 #define DEBUG_PRINT(fmt, args...)
#endif

#define R_DEBUG_REASON_MACH_RCV_INTERRUPTED -2

typedef struct _exc_msg {
	mach_msg_header_t hdr;
	/* start of the kernel processed data */
	mach_msg_body_t msg_body;
	mach_msg_port_descriptor_t thread;
	mach_msg_port_descriptor_t task;
	/* end of the kernel processed data */
	NDR_record_t NDR;
	exception_type_t exception;
	mach_msg_type_number_t code_cnt;
	/* some times RCV_TO_LARGE probs */
	char pad[512];
} exc_msg;

typedef struct _exception_info {
	exception_mask_t masks[EXC_TYPES_COUNT];
	mach_port_t ports[EXC_TYPES_COUNT];
	exception_behavior_t behaviors[EXC_TYPES_COUNT];
	thread_state_flavor_t flavors[EXC_TYPES_COUNT];
	mach_msg_type_number_t count;
	pthread_t thread;
	mach_port_t exception_port;
} xnu_exception_info;

typedef enum {
	R_DEBUG_REASON_DEAD = -1,
	R_DEBUG_REASON_NONE = 0,
	R_DEBUG_REASON_SIGNAL,
	R_DEBUG_REASON_SEGFAULT,
	R_DEBUG_REASON_BREAKPOINT,
	R_DEBUG_REASON_TRACEPOINT,
	R_DEBUG_REASON_COND,
	R_DEBUG_REASON_READERR,
	R_DEBUG_REASON_STEP,
	R_DEBUG_REASON_ABORT,
	R_DEBUG_REASON_WRITERR,
	R_DEBUG_REASON_DIVBYZERO,
	R_DEBUG_REASON_ILLEGAL,
	R_DEBUG_REASON_UNKNOWN,
	R_DEBUG_REASON_ERROR,
	R_DEBUG_REASON_NEW_PID,
	R_DEBUG_REASON_NEW_TID,
	R_DEBUG_REASON_NEW_LIB,
	R_DEBUG_REASON_EXIT_PID,
	R_DEBUG_REASON_EXIT_TID,
	R_DEBUG_REASON_EXIT_LIB,
	R_DEBUG_REASON_TRAP,
	R_DEBUG_REASON_SWI,
	R_DEBUG_REASON_INT,
	R_DEBUG_REASON_FPU,
	R_DEBUG_REASON_USERSUSP,
} RDebugReasonType;

static task_t task_dbg = 0;
static xnu_exception_info ex = { { 0 } };

int xnu_wait(pid_t pid);
static int handle_exception_message(exc_msg *msg, int *ret_code);
static bool validate_mach_message(pid_t pid, exc_msg *msg);
static void encode_reply(mig_reply_error_t *reply, mach_msg_header_t *hdr, int code);
static bool handle_dead_notify(exc_msg *msg);
RDebugReasonType debug_wait(pid_t pid);
task_t pid_to_task (int pid);

int xnu_attach(pid_t pid);
int xnu_detach(pid_t pid);
int xnu_stop(pid_t pid);
bool create_exception_thread(pid_t pid);
static bool restore_exception_ports (int pid);
static int task_suspend_count(task_t task);

#endif
