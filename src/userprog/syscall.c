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
int add_file (struct file *f);
struct file *get_file_from_fd (int fd);
int close_file (int fd);


/* lock */
struct lock file_system_lock;
struct file_entry
{
	int fd;
	struct file *file;
	struct list_elem elem;
};

void
syscall_init (void) 
{
	lock_init(&file_system_lock);
	intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}

static void
syscall_handler (struct intr_frame *f UNUSED) 
{
	int syscall_code = *(int*) f->esp;
	int arg[20];

	printf ("\r\n");
	hex_dump((uintptr_t)f->esp, (f->esp),150, true);
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
			get_arguments_from_stack(f,&arg,1);
                        f->eax  = filesize((int)arg[0]);
			break;

		case SYS_READ:
			get_arguments_from_stack(f,&arg,3);
                        f->eax  = read((int)arg[0],(void *)arg[1],(size_t)arg[2]);
			break;

		case SYS_WRITE:
			get_arguments_from_stack(f,&arg,3);
			f->eax  = write ((int)arg[0],(const void *)arg[1],(size_t)arg[2]);
			break;

		case SYS_SEEK:
			get_arguments_from_stack(f,&arg,2);
                        seek ((int)arg[0],(size_t)arg[1]);
			break;

		case SYS_TELL:
			get_arguments_from_stack(f,&arg,1);
                        f->eax  = tell((int)arg[0]);
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

/* Pratibha sys call starts */

size_t 
write(int fd, const void *buf, size_t count)
{
	if (fd == 1)
	{
		putbuf (buf,count);
		return count;
	}
	lock_acquire(&file_system_lock);
	struct file *filename= get_file_from_fd(fd);

	if(filename==NULL)
	{
		lock_release(&file_system_lock);
		return -1;
	}	

	size_t count_t=file_write(filename,buf,count);
	lock_release(&file_system_lock);
    	return count_t; 
}


size_t 
read (int fd, void *buf, size_t count)
{
	if (fd == 0)
	{
		size_t i;
		for(i=0;i<count;i++)
		{
			(char*)buf[i]=input_getc();
		}		
		return i;
	}

	lock_acquire(&file_system_lock);
	struct file *filename= get_file_from_fd(fd);

	if(filename==NULL)
	{
		lock_release(&file_system_lock);
		return -1;
	}	

	size_t count_t=file_read(filename,buf,count);
	lock_release(&file_system_lock);
	return count_t; 
}

void seek (int fd, size_t position)
{
	lock_acquire(&file_system_lock);
	struct file *filename= get_file_from_fd(fd);

	if(filename==NULL)
	{
		lock_release(&file_system_lock);
		return;
	}	

	file_seek(filename,position);
	lock_release(&file_system_lock);	
}


size_t tell (int fd)
{
	lock_acquire(&file_system_lock);
	struct file *filename= get_file_from_fd(fd);

	if(filename==NULL)
	{
		lock_release(&file_system_lock);
		return -1;
	}	

	size_t position=file_tell(filename);
	lock_release(&file_system_lock);	
	return position;
}

size_t filesize (int fd)
{
	lock_acquire(&file_system_lock);
	struct file *filename= get_file_from_fd(fd);
	
	if(filename==NULL)
	{
		lock_release(&file_system_lock);
		return -1;
	}	

	size_t length=file_length(filename);
	lock_release(&file_system_lock);	
	return length;	
}


struct file* get_file_from_fd(int fd)
{
	struct thread *cur_thread=thread_current();
	struct list_elem  *e;

	for (e = list_begin (&cur_thread->open_files); e!=list_end(&cur_thread->open_files);e=list_next(e))
	{
		  struct file_entry *f = list_entry (e, struct file_entry, elem);
		  if(f->fd==fd)
		  	return (f->file);
	}
	return NULL;
 }

/* Pratibha sys call ends */





int add_file (struct file *f)
{
	/* malloc the entry 
	   populate it
	   update the pointers
	   push it to the list
	   return the ffd 
	  */

}


int close_file (int fd)
{
	/* wiht fd loop through the thread
	find the entry 
	remove the file form the list and close
	*/
}
