#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"

/* Syscall declarations */
size_t  write(int fd, const void *buf, size_t count);
void exit(int);

/* Other function declarations */
static void syscall_handler (struct intr_frame *);
void get_arguments_from_stack (struct intr_frame *f, int *arg, int n);

void
syscall_init (void) 
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}

static void
syscall_handler (struct intr_frame *f UNUSED) 
{
	int syscall_code = *(int*) f->esp;
	int arg[20];

	//hex_dump((uintptr_t)f->esp, (f->esp),150, true);
	//printf("In SysCall Handler Code : %d\n", code);
	//printf ("Pointers *esp  esp %x \r\n", f->esp);
	switch (syscall_code) {
		case SYS_HALT:
			system_halt ();
			break;

		case SYS_EXIT:
			get_arguments_from_stack(f, &arg,1);
			exit(arg[0]);
			break;

		case SYS_EXEC:
			break;

		case SYS_WAIT:
			break;

		case SYS_CREATE:
			break;

		case SYS_REMOVE:
			break;

		case SYS_OPEN:
			break;

		case SYS_FILESIZE:
			break;

		case SYS_READ:
			break;

		case SYS_WRITE:
			get_arguments_from_stack(f,&arg,3);
			f->eax  = write ((int)arg[0],(const void *)arg[1],(size_t)arg[2]);
			break;

		case SYS_SEEK:
			break;

		case SYS_TELL:
			break;

		case SYS_CLOSE:
			break;
			
		default:
			/* To be implemented */
			break;
	}
}


void get_arguments_from_stack (struct intr_frame *f, int *arg, int n)
{
	for (int i = 0; i < n; i++)
    {
		arg[i] = *((int *)(f->esp) + i + 1);
    }
}

/* We have tried to mimic all the syscalls similar to Linux. */
/* SYS_HALT */
void system_halt(void)
{
	shutdown_power_off();
	return;
}


/* SYS_EXIT */
void exit(int status)
{
	/* VENKAT - Feel more code is required here */ 
	printf ("%s: exit(%d)\n", thread_current()->name, status);
	thread_exit();
}

size_t 
write(int fd, const void *buf, size_t count)
{
	if (fd == 1)
	{
		putbuf (buf,count);
		return count;
	}
	/* more core to be added */
	putbuf (buf,count);
	return count; 
}
