#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "filesys/filesys.h"
#include "filesys/file.h"

/* Syscall declarations */
size_t  write(int fd, const void *buf, size_t count);
void exit(int);
bool create_file(const char *file_name, off_t initial_size);
int open_file(const char *file_name);
void close_file(int fd);
bool remove_file(const char *file_name);

/* Other function declarations */
static void syscall_handler (struct intr_frame *); 
void get_arguments_from_stack (struct intr_frame *f, int *arg, int n); 
int add_file (struct file *f);
struct file *get_file_from_fd (int fd);


/* lock */
struct lock *file_system_lock;

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
			get_arguments_from_stack(f,&arg,2);
			lock_acquire(&file_system_lock);
			f->eax = create_file((const char *)arg[0], (off_t) arg[1]);
			lock_release(&file_system_lock);
			break;

		case SYS_REMOVE:
			get_arguments_from_stack(f,&arg,1);
			lock_acquire(&file_system_lock);
			f->eax = remove_file((const char *)arg[0]);
			lock_release(&file_system_lock);
			break;

		case SYS_OPEN:
			get_arguments_from_stack(f,&arg,1);
			lock_acquire(&file_system_lock);
			f->eax = open_file((const char *)arg[0]);
			lock_release(&file_system_lock);
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
			get_arguments_from_stack(f,&arg,1);
			lock_acquire(&file_system_lock);
			close_file((int)arg[0]);
			lock_release(&file_system_lock);
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

	lock_acquire(&file_system_lock);
	putbuf (buf,count);
	lock_release(&file_system_lock);

	return count; 
}

/* Create a file and return true if successful
   else false */
bool 
create_file(const char *file_name, off_t initial_size)
{
	return filesys_create(file_name, initial_size);	
}

/* Removes a file and returns true if successful
   else false */
bool
remove_file(const char *file_name)
{
	return filesys_remove(file_name);
}

int add_file (struct file *f)
{
	/* malloc the entry 
	   populate it
	   update the pointers
	   push it to the list
	   return the ffd 
	  */

}

struct file *
get_file_from_fd (int fd)
{
	
	/* loop through the list entry o the threaadd
		if some list entry matches return the file descriptor
		*/
}

/* Closes a file and removes the fd from process fd list */
void close_file (int fd)
{
	struct thread *t = thread_current();
	struct list_elem *e;
	for (e = list_begin (&t->open_files); e != list_end (&t->open_files);
         e = list_next (e))
    {
        struct file_entry *f = list_entry (e, struct file_entry, elem);
		if(f->fd == fd)
		{
			file_close(f->file);
			list_remove	(e);
			break;
		}
	}

}

/* Opens a file and returns a fd if successful
   else -1 */
int
open_file(const char *file_name)
{
	struct file *file = filesys_open(file_name);
	if(file == NULL)
		return -1;

	else
	{
		struct thread *t1 = thread_current();
		struct file_entry *f1;
		f1->fd = t1->next_fd;
		t1->next_fd++;
		f1->file = file;
		list_push_back(&t1->open_files,&f1->elem);
		return f1->fd;
	}
}





