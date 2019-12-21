#include "msg.h"

int xnu_wait(pid_t pid) {
	kern_return_t kr;
	int ret_code, reason = R_DEBUG_REASON_UNKNOWN;
	mig_reply_error_t reply;
	bool ret;
	exc_msg msg;

	msg.hdr.msgh_local_port = ex.exception_port;
	msg.hdr.msgh_size = sizeof (exc_msg);
	for (;;) {
		DEBUG_PRINT("Sending RCV message\n");
		kr = mach_msg (&msg.hdr, MACH_RCV_MSG | MACH_RCV_INTERRUPT, 0, sizeof (exc_msg), ex.exception_port, MACH_MSG_TIMEOUT_NONE, MACH_PORT_NULL);
		if (kr == MACH_RCV_INTERRUPTED) {
			reason = R_DEBUG_REASON_MACH_RCV_INTERRUPTED;
			break;
		} else if (kr != MACH_MSG_SUCCESS) {
			DEBUG_PRINT("message didn't succeeded\n"); //%#010x\n",kr);
			break;
		}
		DEBUG_PRINT("Validationg message...\n");
		ret = validate_mach_message(pid, &msg);
		if (!ret) {
			DEBUG_PRINT("Handling dead...\n");
			ret = handle_dead_notify(&msg);
			if (ret) {
				DEBUG_PRINT("Reason = dead...\n");
				reason = R_DEBUG_REASON_DEAD;
				break;
			}
		}
		if (!ret) {
			DEBUG_PRINT("Encoding reply...\n");
			encode_reply (&reply, &msg.hdr, KERN_FAILURE);
			DEBUG_PRINT("Sending SND message...\n");
			kr = mach_msg (&reply.Head, MACH_SEND_MSG | MACH_SEND_INTERRUPT, reply.Head.msgh_size, 0, MACH_PORT_NULL, MACH_MSG_TIMEOUT_NONE, MACH_PORT_NULL);
			if (reply.Head.msgh_remote_port != 0 && kr != MACH_MSG_SUCCESS) {
				DEBUG_PRINT("Deatllocaiton task...\n");
				kr = mach_port_deallocate(mach_task_self(), reply.Head.msgh_remote_port);
				if (kr != KERN_SUCCESS) {
					DEBUG_PRINT("failed to deallocate reply port\n");
				}
			}
			DEBUG_PRINT("SND Message sent successfully ...\n");
			continue;
		}
		DEBUG_PRINT("Handling exception message...\n");
		reason = handle_exception_message(&msg, &ret_code);
		DEBUG_PRINT("Encoding reply...\n");
		encode_reply (&reply, &msg.hdr, ret_code);
		DEBUG_PRINT("Sending SND message 2...\n");
		kr = mach_msg (&reply.Head, MACH_SEND_MSG | MACH_SEND_INTERRUPT, reply.Head.msgh_size, 0,
				MACH_PORT_NULL, 0,
				MACH_PORT_NULL);
		if (reply.Head.msgh_remote_port != 0 && kr != MACH_MSG_SUCCESS) {
			DEBUG_PRINT("Deatllocaiton task 2...\n");
			kr = mach_port_deallocate(mach_task_self (), reply.Head.msgh_remote_port);
			if (kr != KERN_SUCCESS)
				DEBUG_PRINT("failed to deallocate reply port\n");
		}
		DEBUG_PRINT("SND Message 2 sent successfully ...\n");
		break; // to avoid infinite loops
	}
	DEBUG_PRINT("Returning wait...\n");
	return reason;
}

static int handle_exception_message(exc_msg *msg, int *ret_code) {
	int ret = R_DEBUG_REASON_UNKNOWN;
	kern_return_t kr;
	*ret_code = KERN_SUCCESS;
	switch (msg->exception) {
	case EXC_BAD_ACCESS:
		ret = R_DEBUG_REASON_SEGFAULT;
		*ret_code = KERN_FAILURE;
		kr = task_suspend (msg->task.name);
		if (kr != KERN_SUCCESS) {
			DEBUG_PRINT("failed to suspend task bad access\n");
		}
		printf("EXC_BAD_ACCESS\n");
		break;
	case EXC_BAD_INSTRUCTION:
		ret = R_DEBUG_REASON_ILLEGAL;
		*ret_code = KERN_FAILURE;
		kr = task_suspend (msg->task.name);
		if (kr != KERN_SUCCESS) {
			DEBUG_PRINT("failed to suspend task bad instruction\n");
		}
		printf("EXC_BAD_INSTRUCTION\n");
		break;
	case EXC_ARITHMETIC:
		printf("EXC_ARITHMETIC\n");
		break;
	case EXC_EMULATION:
		printf("EXC_EMULATION\n");
		break;
	case EXC_SOFTWARE:
		printf("EXC_SOFTWARE\n");
		break;
	case EXC_BREAKPOINT:
		kr = task_suspend (msg->task.name);
		if (kr != KERN_SUCCESS) {
			DEBUG_PRINT("failed to suspend task breakpoint\n");
		}
		ret = R_DEBUG_REASON_BREAKPOINT;
		break;
	default:
		printf("UNKNOWN\n");
		break;
	}
	kr = mach_port_deallocate (mach_task_self (), msg->task.name);
	if (kr != KERN_SUCCESS) {
		DEBUG_PRINT("failed to deallocate task port\n");
	}
	kr = mach_port_deallocate (mach_task_self (), msg->thread.name);
	if (kr != KERN_SUCCESS) {
		DEBUG_PRINT("failed to deallocated task port\n");
	}
	return ret;
}

static bool validate_mach_message(pid_t pid, exc_msg *msg) {
	kern_return_t kr;
	/*check if the message is for us*/
	if (msg->hdr.msgh_local_port != ex.exception_port) {
		return false;
	}
	/*gdb from apple check this so why not us*/
	if (!(msg->hdr.msgh_bits & MACH_MSGH_BITS_COMPLEX)) {
		return false;
	}
	/*mach exception we are interested*/
	//XXX for i386 this id seems to be different
	if (msg->hdr.msgh_id > 2405 || msg->hdr.msgh_id < 2401) {
		return false;
	}
	/* check descriptors.  */
	if (msg->hdr.msgh_size <
	    sizeof (mach_msg_header_t) + sizeof (mach_msg_body_t) +
		    2 * sizeof (mach_msg_port_descriptor_t) +
		    sizeof (NDR_record_t) + sizeof (exception_type_t) +
		    sizeof (mach_msg_type_number_t) +
		    sizeof (mach_exception_data_t))
		return false;
	/* check data representation.  */
	if (msg->NDR.mig_vers != NDR_PROTOCOL_2_0 ||
	    msg->NDR.if_vers != NDR_PROTOCOL_2_0 ||
	    msg->NDR.mig_encoding != NDR_record.mig_encoding ||
	    msg->NDR.int_rep != NDR_record.int_rep ||
	    msg->NDR.char_rep != NDR_record.char_rep ||
	    msg->NDR.float_rep != NDR_record.float_rep) {
		return false;
	}
	if (pid_to_task(pid) != msg->task.name) {
		//we receive a exception from an unknown process this could
		//happen if the child fork, as the created process will inherit
		//its exception port
		/*we got new rights to the task, get rid of it.*/
		kr = mach_port_deallocate (mach_task_self (), msg->task.name);
		if (kr != KERN_SUCCESS) {
			DEBUG_PRINT("validate_mach_message: failed to deallocate task port\n");
		}
		kr = mach_port_deallocate (mach_task_self (), msg->thread.name);
		if (kr != KERN_SUCCESS) {
			DEBUG_PRINT("validate_mach_message2: failed to deallocated task port\n");
		}
		return false;
	}
	return true;
}

static void encode_reply(mig_reply_error_t *reply, mach_msg_header_t *hdr, int code) {
	mach_msg_header_t *rh = &reply->Head;
	rh->msgh_bits = MACH_MSGH_BITS (MACH_MSGH_BITS_REMOTE(hdr->msgh_bits), 0);
	rh->msgh_remote_port = hdr->msgh_remote_port;
	rh->msgh_size = (mach_msg_size_t) sizeof (mig_reply_error_t);
	rh->msgh_local_port = MACH_PORT_NULL;
	rh->msgh_id = hdr->msgh_id + 100;
	reply->NDR = NDR_record;
	reply->RetCode = code;
}

static bool handle_dead_notify(exc_msg *msg) {
	if (msg->hdr.msgh_id == 0x48) {
		return true;
	}
	return false;
}

RDebugReasonType debug_wait(pid_t pid) {
	RDebugReasonType reason = R_DEBUG_REASON_ERROR;

	reason = xnu_wait(pid);

	if (reason == R_DEBUG_REASON_DEAD) { printf("R_DEBUG_REASON_DEAD ==> Process finished\n\n"); }
	if (reason == R_DEBUG_REASON_ERROR) { printf("R_DEBUG_REASON_ERROR\n"); }
	if (reason == R_DEBUG_REASON_BREAKPOINT || reason == R_DEBUG_REASON_STEP) {
		DEBUG_PRINT("Just hit breakponint (breakpoint handling missing)");

		// === BREAKPOINT HANDLING ==
		// get RIP register
		// get value under RIP register
		// handle breakpoint hit: remove breakpoint and fix RIP register - r_debug_bp_hit
		// LATER on resume: always step once, then put back breakpoint - r_debug_recoil
	}
	if (reason == R_DEBUG_REASON_SIGNAL) { printf("R_DEBUG_REASON_SIGNAL (can be SIGTRAP)\n"); }

	DEBUG_PRINT("Returning reason\n");
	return reason;
}

task_t pid_to_task (int pid) {
	static int old_pid = -1;
	kern_return_t kr;
	task_t task = -1;
	int err;
	/* it means that we are done with the task*/
	if (task_dbg != 0 && old_pid == pid) {
		return task_dbg;
	}
	if (task_dbg != 0 && old_pid != pid) {
		//we changed the process pid so deallocate a ref from the old_task
		//since we are going to get a new task
		kr = mach_port_deallocate (mach_task_self (), task_dbg);
		if (kr != KERN_SUCCESS) {
			DEBUG_PRINT("pid_to_task: fail to deallocate port\n");
			/* ignore on purpose to not break process reload: ood */
			//return 0;
		}

	}
	err = task_for_pid (mach_task_self (), (pid_t)pid, &task);
	if ((err != KERN_SUCCESS) || !MACH_PORT_VALID (task)) { //BRAK
        DEBUG_PRINT("task_for_pid: fail to find task\n");
        return 0;
	}
	old_pid = pid;
	task_dbg = task;
	return task;
}

int xnu_attach(pid_t pid) {
	if (!create_exception_thread(pid)) {
		DEBUG_PRINT("error setting up exception thread\n");
        return -1;
	}
	xnu_stop(pid);
	return pid;
}

int xnu_detach(pid_t pid) {
    kern_return_t kr;
	//do the cleanup necessary
	//XXX check for errors and ref counts
	(void)restore_exception_ports (pid);
	kr = mach_port_deallocate (mach_task_self (), task_dbg);
	if (kr != KERN_SUCCESS) {
		DEBUG_PRINT("xnu_detach: failed to deallocate port\n");
		return false;
	}
	//we mark the task as not longer available since we deallocated the ref
	task_dbg = 0;
	return true;
}

int xnu_stop(pid_t pid) {
	task_t task = pid_to_task (pid);
	if (!task) {
		return false;
	}

	int suspend_count = task_suspend_count (task);
	if (suspend_count == -1) {
		return false;
	}
	if (suspend_count == 1) {
		// Hopefully _we_ suspended it.
		return true;
	}
	if (suspend_count > 1) {
		// This is unexpected.
		return false;
	}

	kern_return_t kr = task_suspend (task);
	if (kr != KERN_SUCCESS) {
		DEBUG_PRINT("failed to suspend task\n");
		return false;
	}

	suspend_count = task_suspend_count (task);
	if (suspend_count != 1) {
		// This is unexpected.
		return false;
	}
	return true;
}
bool create_exception_thread(pid_t pid) {
	kern_return_t kr;
	mach_port_t exception_port = MACH_PORT_NULL;
	mach_port_t req_port;
        // Got the mach port for the current process
	mach_port_t task_self = mach_task_self();
	task_t task = pid_to_task(pid);

	if (!task) {
		DEBUG_PRINT("error to get task for the debuggee process xnu_start_exception_thread\n");
		return false;
	}
	// r_debug_ptrace (dbg, PT_ATTACHEXC, pid, 0, 0);
    ptrace (PT_ATTACHEXC, pid, 0, 0); // Doublecheck if needed

	if (!MACH_PORT_VALID (task_self)) {
		DEBUG_PRINT("error to get the task for the current process xnu_start_exception_thread\n");
		return false;
	}
        // Allocate an exception port that we will use to track our child process
    kr = mach_port_allocate (task_self, MACH_PORT_RIGHT_RECEIVE, &exception_port);
	if (kr != KERN_SUCCESS) {
		mach_error ("error to allocate mach_port exception\n", kr);
		return false;
	}
	// RETURN_ON_MACH_ERROR ("error to allocate mach_port exception\n", false);

        // Add the ability to send messages on the new exception port
	kr = mach_port_insert_right (task_self, exception_port, exception_port, MACH_MSG_TYPE_MAKE_SEND);
	if (kr != KERN_SUCCESS) {
		mach_error ("error to allocate insert right\n", kr);
		return false;
	}
	// RETURN_ON_MACH_ERROR ("error to allocate insert right\n", false);

    	// Atomically swap out (and save) the child process's exception ports
    	// for the one we just created. We'll want to receive all exceptions.
	ex.count = (sizeof (ex.ports) / sizeof (*ex.ports));
	kr = task_swap_exception_ports (task, EXC_MASK_ALL, exception_port, EXCEPTION_DEFAULT | MACH_EXCEPTION_CODES, THREAD_STATE_NONE,
		ex.masks, &ex.count, ex.ports, ex.behaviors, ex.flavors);
	if (kr != KERN_SUCCESS) {
		mach_error ("failed to swap exception ports\n", kr);
		return false;
	}
	// RETURN_ON_MACH_ERROR ("failed to swap exception ports\n", false);

	   // Get notification when process die
	kr = mach_port_request_notification (task_self, task, MACH_NOTIFY_DEAD_NAME, 0, exception_port, MACH_MSG_TYPE_MAKE_SEND_ONCE, &req_port);

	if (kr != KERN_SUCCESS) {
		DEBUG_PRINT("Termination notification request failed\n");
	}

	ex.exception_port = exception_port;
	return true;
}

static bool restore_exception_ports (int pid) {
	kern_return_t kr;
	int i;
	task_t task = pid_to_task (pid);
	if (!task)
		return false;
	for (i = 0; i < ex.count; i++) {
		kr = task_set_exception_ports (task, ex.masks[i], ex.ports[i], ex.behaviors[i], ex.flavors[i]);
		if (kr != KERN_SUCCESS) {
			DEBUG_PRINT("fail to restore exception ports\n");
			return false;
		}
	}
	kr = mach_port_deallocate (mach_task_self (), ex.exception_port);
	if (kr != KERN_SUCCESS) {
		DEBUG_PRINT("failed to deallocate exception port\n");
		return false;
	}
	return true;
}

static int task_suspend_count(task_t task) {
	kern_return_t kr;
	struct task_basic_info info;
	mach_msg_type_number_t count = TASK_BASIC_INFO_COUNT;
	kr = task_info (task, TASK_BASIC_INFO, (task_info_t) &info, &count);
	if (kr != KERN_SUCCESS) {
		DEBUG_PRINT("failed to get task info\n");
		return -1;
	}
	return info.suspend_count;
}
