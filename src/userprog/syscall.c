#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "threads/vaddr.h"

#define USER_BOTTOM_ADDR ((void *) 0x08048000)

/* Syscall declarations */
void exit(int);
int exec(const char *string);
int wait (int pid);
size_t write(int fd, const void *buf, size_t count);
size_t read (int fd, void *buf, size_t count);
void seek (int fd, size_t position);
size_t tell (int fd);
size_t filesize (int fd);
bool create_file(const char *file_name, int initial_size);
int open_file(const char *file_name);
void close_file(int fd);
bool remove_file(const char *file_name);

/* Other function declarations */
static void syscall_handler (struct intr_frame *);
void get_arguments_from_stack (struct intr_frame *f, int *arg, int n);
struct file* get_file_from_fd(int fd);
void filesys_lock();
void filesys_unlock();

/* lock */
struct lock file_system_lock;

void
syscall_init (void) 
{
	lock_init(&file_system_lock);
	intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}

static void
syscall_handler (struct intr_frame *f UNUSED) 
{
/*
  
  	if (!is_user_vaddr (p))
    		exit(-1);
  
 	if (*p < SYS_HALT || *p > SYS_INUMBER)
		exit(-1);

	if (!(is_user_vaddr (p + 1) && is_user_vaddr (p + 2) && is_user_vaddr (p + 3)))
		exit(-1);
*/
	int syscall_code = *(int*) f->esp;
	int arg[20];
	//printf ("In syscall hanlder <%d>\r\n",syscall_code);
	//printf ("\r\n");
	//hex_dump((uintptr_t)f->esp, (f->esp),150, true);

	//printf ("\r\n");
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
			get_arguments_from_stack (f, &arg, 1);
			arg[0] = check_pointers((const void *) arg[0]);     // extra
			f->eax = exec (arg[0]);
			break;

		case SYS_WAIT:
			get_arguments_from_stack (f, &arg, 1);
			f->eax = wait(arg[0]);
			break;

		case SYS_FILESIZE:
			get_arguments_from_stack(f,&arg,1);
            		f->eax  = filesize((int)arg[0]);
			break;

		case SYS_READ:
			get_arguments_from_stack(f,&arg,3);
            		buffer_validity((void *)arg[1], (int) arg[2]);        //extra
			arg[1] = check_pointers((const void *) arg[1]);   //extra
            		f->eax  = read((int)arg[0],(void *)arg[1],(size_t)arg[2]);
			break;

		case SYS_WRITE:
			get_arguments_from_stack(f,&arg,3);
			buffer_validity((void *)arg[1], (int) arg[2]);		//extra
			arg[1] = check_pointers((const void *) arg[1]);	//extra
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
			get_arguments_from_stack(f,&arg,1);
			lock_acquire(&file_system_lock);
			close_file((int)arg[0]);
			lock_release(&file_system_lock);
			break;
			
		case SYS_CREATE:
			get_arguments_from_stack(f,&arg,2);
			arg[0] = check_pointers((const void *) arg[0]);    //extra
			lock_acquire(&file_system_lock);
			f->eax = create_file((const char *)arg[0], (int) arg[1]);
			lock_release(&file_system_lock);
			break;

		case SYS_REMOVE:
			get_arguments_from_stack(f,&arg,1);
			arg[0] = check_pointers((const void *) arg[0]);     //extra
			lock_acquire(&file_system_lock);
			f->eax = remove_file((const char *)arg[0]);
			lock_release(&file_system_lock);
			break;

		case SYS_OPEN:
			get_arguments_from_stack(f,&arg,1);
			arg[0] = check_pointers((const void *) arg[0]);   //extra
			lock_acquire(&file_system_lock);
			f->eax = open_file((const char *)arg[0]);
			lock_release(&file_system_lock);
			break;

			
		default:
			/* To be implemented */
			break;
	}
}

int wait (int process_id)
{
	if (process_id  > 300)
	{
		exit(ERROR);
	}

	return process_wait(process_id);
}

void get_arguments_from_stack (struct intr_frame *f, int *arg, int n)
{
	int *temp;

	for (int i = 0; i < n; i++)
    {
    	temp = *((int *)(f->esp) + i + 1);
    	pointer_validity ((const void *) temp);
		arg[i] = *((int *)(f->esp) + i + 1);
    }
    return;
}

void pointer_validity (const void *ptr)
{
	/* venkat more checks may be required */
    if (!is_user_vaddr(ptr))
    {
		exit(ERROR);
    }
#if 0
    void *temp_ptr = NULL;
    temp_ptr = pagedir_get_page(thread_current()->pagedir, ptr);
	if (!temp_ptr)
	{
		printf ("Erroring out here \r\n");
		exit_proc(-1);
		return 0;
	}
#endif
    return;
}

void buffer_validity (void* buffer, size_t size)
{
	char* local_buffer = (char *) buffer;
  	for ( size_t i = 0; i < size; i++)
  	{
		 if (!is_user_vaddr(local_buffer) || local_buffer < USER_BOTTOM_ADDR)
    		{
     			 exit(ERROR);
	        }

  		local_buffer++;
  	}
  	return;
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

int
exec (const char *cmd_line)
{
	if (cmd_line == NULL)
		exit(ERROR);

	int pid = process_execute(cmd_line);
	return pid;
}



void exit_proc(int status)
{
	//printf("Exit : %s %d %d\n",thread_current()->name, thread_current()->tid, status);
	struct list_elem *e;
	struct thread *td= thread_current();

      for (e = list_begin (&(td->parent->child_list)); e != list_end (&(td->parent->child_list));
           e = list_next (e))
        {
          struct child_entry *f = list_entry (e, struct child_entry, elem);
          if(f->tid == td->tid)
          {
          	f->used = true;
          	f->exit_error = status;
          }
        }

	thread_current()->exit_error = status;

	if(thread_current()->parent->waitingon == thread_current()->tid)
		sema_up(&thread_current()->parent->child_lock);

	thread_exit();
}


/* Pratibha sys call starts */
size_t 
write(int fd, const void *buf, size_t count)
{
	//printf ("In write system call:<%d><%s><%d>\r\n",fd,buf,count);
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
	if (buf == NULL)
		exit(ERROR);

	if (fd == 0)
	{
		size_t i;
		uint8_t* temp=(uint8_t *) buf; 
		for(i=0;i<count;i++)
		{
			temp[i]=input_getc();
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
		if(e==NULL)
			continue;

		struct file_entry *f = list_entry (e, struct file_entry, elem);
		if(f==NULL)
			continue;

		if(f->fd==fd)
		  	return (f->file);
	}
	return NULL;
}


int check_pointers(const void *ptr_addr)
{
	 if (!is_user_vaddr(ptr_addr) || ptr_addr < USER_BOTTOM_ADDR)
   	 {
     		 exit(ERROR);
   	 }

  	void *pointer = pagedir_get_page(thread_current()->pagedir, ptr_addr);
  	if (!pointer)
    	{
     		 exit(ERROR);
   	}
  	return (int) pointer;
}


/* Pratibha sys call ends */

/* Create a file and return true if successful
   else false */
bool 
create_file(const char *file_name, int initial_size)
{
	if (file_name == NULL)
	{
		lock_release(&file_system_lock);
		exit(ERROR);
	}

	return filesys_create(file_name, initial_size);	
}

/* Removes a file and returns true if successful
   else false */
bool
remove_file(const char *file_name)
{
	return filesys_remove(file_name);
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
	if (file_name == NULL)
	{
		lock_release(&file_system_lock);
		exit(ERROR);
	}

	struct file *file = filesys_open(file_name);
	if(file == NULL)
		return -1;

	else
	{
		struct thread *t1 = thread_current();
		struct file_entry *f1 = malloc(sizeof(struct file_entry));
		f1->fd = t1->next_fd;
		t1->next_fd++;
		f1->file = file;
		list_push_back(&t1->open_files,&f1->elem);
		return f1->fd;
	}
}

/* this is sort of api willl be used from outisde */
void 
filesys_lock()
{
	lock_acquire(&file_system_lock);
}

void
filesys_unlock()
{
	lock_release(&file_system_lock);
}
