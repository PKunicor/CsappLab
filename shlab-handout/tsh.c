/*
 * tsh - A tiny shell program with job control
 *
 * <Put your name and login ID here>
 */
#include <ctype.h>
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>


/* Misc manifest constants */
#define MAXLINE 1024   /* max line size */
#define MAXARGS 128    /* max args on a command line */
#define MAXJOBS 16     /* max jobs at any point in time */
#define MAXJID 1 << 16 /* max job ID */

/* Job states */
#define UNDEF 0 /* undefined */
#define FG 1    /* running in foreground */
#define BG 2    /* running in background */
#define ST 3    /* stopped */

/*
 * Jobs states: FG (foreground), BG (background), ST (stopped)
 * Job state transitions and enabling actions:
 *     FG -> ST  : ctrl-z
 *     ST -> FG  : fg command
 *     ST -> BG  : bg command
 *     BG -> FG  : fg command
 * At most 1 job can be in the FG state.
 */

/* Global variables */
extern char **environ;   /* defined in libc */
char prompt[] = "tsh> "; /* command line prompt (DO NOT CHANGE) */
int verbose = 0;         /* if true, print additional output */
int nextjid = 1;         /* next job ID to allocate */
char sbuf[MAXLINE];      /* for composing sprintf messages */

struct job_t {           /* The job struct */
  pid_t pid;             /* job PID */
  int jid;               /* job ID [1, 2, ...] */
  int state;             /* UNDEF, BG, FG, or ST */
  char cmdline[MAXLINE]; /* command line */
};
struct job_t jobs[MAXJOBS]; /* The job list */
/* End global variables */


/* Function prototypes */

//补充函数
pid_t Fork(void);                                                              //封装fork
void Execve(const char *filename, char *const argv[], char *const environ[]);  //封装execv函数


/* Here are the functions that you will implement */
void eval(char *cmdline);  //分析命令
int builtin_cmd(char **argv);
void do_bgfg(char **argv);
void waitfg(pid_t pid);

void sigchld_handler(int sig);
void sigtstp_handler(int sig);
void sigint_handler(int sig);

/* Here are helper routines that we've provided for you */
int parseline(const char *cmdline, char **argv);  //返回bg标志  1 后台
void sigquit_handler(int sig);

void clearjob(struct job_t *job);                                     //清除队列中的一个作业
void initjobs(struct job_t *jobs);                                    //初始化
int maxjid(struct job_t *jobs);                                       //返回以分配的最大作业号
int addjob(struct job_t *jobs, pid_t pid, int state, char *cmdline);  //添加一个作业  返回1  满了返回0
int deletejob(struct job_t *jobs, pid_t pid);                         //删除队列中一个作业   成功返回1
pid_t fgpid(struct job_t *jobs);                                      //返回一个前台运行的作业号    没有返回0
struct job_t *getjobpid(struct job_t *jobs, pid_t pid);               //使用pid查找一个作业  没找到 返回空
struct job_t *getjobjid(struct job_t *jobs, int jid);                 //用jid查找一个作业
int pid2jid(pid_t pid);                                               // 用pid 返回 jid,  没找到返回0
void listjobs(struct job_t *jobs);                                    //打印作业队列

void usage(void);                                   //打印帮助菜单
void unix_error(char *msg);                         // unix风格打印错误信息
void app_error(char *msg);                          // app风格的错误信息
typedef void handler_t(int);                        //一个int参数,无返回
handler_t *Signal(int signum, handler_t *handler);  //信号包装器

/*
 * main - The shell's main routine
 */
int main(int argc, char **argv) {
  char c;
  char cmdline[MAXLINE];
  int emit_prompt = 1; /* emit prompt (default) */

  //不用管    重定向输出
  /* Redirect stderr to stdout (so that driver will get all output
   * on the pipe connected to stdout) */
  dup2(1, 2);  //把1的文件描述符 复制给2

  /* Parse the command line */
  while ((c = getopt(argc, argv, "hvp")) != EOF) {
    switch (c) {
      case 'h': /* print help message */
        usage();
        break;
      case 'v': /* emit additional diagnostic info */
        verbose = 1;
        break;
      case 'p':          /* don't print a prompt */
        emit_prompt = 0; /* handy for automatic testing */
        break;
      default:
        usage();
    }
  }

  /* Install the signal handlers */



  /* These are the ones you will need to implement */
  Signal(SIGINT, sigint_handler);   /* ctrl-c */
  Signal(SIGTSTP, sigtstp_handler); /* ctrl-z */
  Signal(SIGCHLD, sigchld_handler); /* Terminated or stopped child */

  /* This one provides a clean way to kill the shell */
  Signal(SIGQUIT, sigquit_handler);

  /* Initialize the job list */
  initjobs(jobs);

  /* Execute the shell's read/eval loop */
  while (1) {
    /* Read command line */
    if (emit_prompt) {
      printf("%s", prompt);
      fflush(stdout);
    }
    if ((fgets(cmdline, MAXLINE, stdin) == NULL) && ferror(stdin))
      app_error("fgets error");
    if (feof(stdin)) { /* End of file (ctrl-d) */
      fflush(stdout);
      exit(0);
    }

    /* Evaluate the command line */
    eval(cmdline);
    fflush(stdout);
    fflush(stdout);
  }

  exit(0); /* control never reaches here */
}

/*
 * eval - Evaluate the command line that the user has just typed in
 *
 * If the user has requested a built-in command (quit, jobs, bg or fg)
 * then execute it immediately. Otherwise, fork a child process and
 * run the job in the context of the child. If the job is running in
 * the foreground, wait for it to terminate and then return.  Note:
 * each child process must have a unique process group ID so that our
 * background children don't receive SIGINT (SIGTSTP) from the kernel
 * when we type ctrl-c (ctrl-z) at the keyboard.
 */
void eval(char *cmdline) {
  char *argv[MAXARGS]; /*Argument list execve() */
  char buf[MAXLINE];   /*Holds modified command line */
  int bg;              /*Should the job run in bg or fg? */
  pid_t pid;           /*Process id */

  sigset_t mask_all, mask_one, prev_one;
  sigfillset(&mask_all);
  sigemptyset(&mask_one);
  sigaddset(&mask_one, SIGCHLD);

  strcpy(buf, cmdline);
  bg = parseline(buf, argv);
  if (argv[0] == NULL)
    return; /* Ignore empty lines */

  if (!builtin_cmd(argv)) {
    sigprocmask(SIG_BLOCK, &mask_one, &prev_one);   //阻塞SIGCHLD

    if ((pid = Fork()) == 0) {         /* Child runs user job */
      setpgid(0, 0);          //单独加入一个组
      sigprocmask(SIG_SETMASK, &prev_one, NULL);  //解除SIGCHLD阻塞 赋值新的mask
      Execve(argv[0], argv, environ);  //执行函数不会返回   会结束运行
    }                                  /* Parent waits for foreground job to terminate */
    //添加作业     FG = 0 + 1 = 1    BG = 1 + 1 = 2
    sigprocmask(SIG_BLOCK, &mask_all, NULL);   //应该是重置作用
    addjob(jobs, pid, bg + 1, cmdline);
    sigprocmask(SIG_SETMASK, &prev_one, NULL); 

    if (!bg) {
      waitfg(pid);   //等待前台运行结束
    }
    else {
      printf("[%d] (%d)  %s", pid2jid(pid), pid, cmdline);
    }
  }
  return;
}

/*
 * parseline - Parse the command line and build the argv array.
 *
 * Characters enclosed in single quotes are treated as a single
 * argument.  Return true if the user has requested a BG job, false if
 * the user has requested a FG job.
 */
int parseline(const char *cmdline, char **argv) {
  static char array[MAXLINE]; /* holds local copy of command line */
  char *buf = array;          /* ptr that traverses command line */
  char *delim;                /* points to first space delimiter */
  int argc;                   /* number of args */
  int bg;                     /* background job? */

  strcpy(buf, cmdline);
  buf[strlen(buf) - 1] = ' ';   /* replace trailing '\n' with space */
  while (*buf && (*buf == ' ')) /* ignore leading spaces */
    buf++;

  /* Build the argv list */
  argc = 0;
  if (*buf == '\'') {
    buf++;
    delim = strchr(buf, '\'');
  }
  else {
    delim = strchr(buf, ' ');
  }

  while (delim) {
    argv[argc++] = buf;
    *delim = '\0';
    buf = delim + 1;
    while (*buf && (*buf == ' ')) /* ignore spaces */
      buf++;

    if (*buf == '\'') {
      buf++;
      delim = strchr(buf, '\'');
    }
    else {
      delim = strchr(buf, ' ');
    }
  }
  argv[argc] = NULL;

  if (argc == 0) /* ignore blank line */
    return 1;

  /* should the job run in the background? */
  if ((bg = (*argv[argc - 1] == '&')) != 0) {
    argv[--argc] = NULL;
  }
  return bg;
}

/*
 * builtin_cmd - If the user has typed a built-in command then execute
 *    it immediately.
 */
int builtin_cmd(char **argv) {
  if (!strcmp(argv[0], "quit"))
    exit(0);
  if (!strcmp(argv[0], "jobs")) {
    listjobs(jobs);
    return 1;
  }
  if (!strcmp(argv[0], "bg") || !strcmp(argv[0], "fg")) {
    do_bgfg(argv);
    return 1;
  }
  if (!strcmp(argv[0], "&"))
    return 1;
  return 0; /* not a builtin command */
}

/*
 * do_bgfg - Execute the builtin bg and fg commands
 */
void do_bgfg(char **argv) {
  int doid;
  pid_t pid;
  pid_t t_pid;
  if(argv[1] == NULL){
    printf("%s command requires PID or %%jobid argument\n", argv[0]);
    return;
  }

  int jflag = 0;  //组标记
  if(argv[1][0] == '%'){
    jflag = 1;
    if(isdigit(argv[1][1]) == 0){
      printf("%s: argument must be a PID or %%jobid\n", argv[0]);
      return;
    }
    doid = atoi(&argv[1][1]);
  }
  else{
    if(isdigit(argv[1][0]) == 0){
      printf("%s: argument must be a PID or %%jobid\n", argv[0]);
      return;
    }
    doid = atoi(&argv[1][0]);
  }


  struct job_t * job;
  if(jflag){
    job = getjobjid(jobs, doid);  //jid
    if(job == NULL){
      printf("%d: No such job\n", doid);
      return;
    }
  }
  else{
    job = getjobpid(jobs, doid);  //pid
    if(job == NULL){
      printf("(%d): No such process\n", doid);
      return;
    }
  }
  if (!strcmp(argv[0], "bg")) {


    kill(-job->pid, SIGCONT); //发送继续命令
    job->state = BG;
    printf("[%d] (%d) %s",job->jid,job->pid,job->cmdline);  //不清楚为什么不换行
  }
  else{
    
    t_pid = fgpid(jobs);   //找当前的前台作业
    if(job->state == ST){
      kill(-job->pid, SIGCONT); //发送继续命令
    }
    job->state = FG;
    pid = job->pid;
    if(t_pid != 0){  //有前台作业 转为后台
      job = getjobpid(jobs, doid);
      job->state = BG;
    }
    waitfg(pid);  //等待前台运行
  }
  return;
}

/*
 * waitfg - Block until process pid is no longer the foreground process
 */
/*
同一时间只有一个fg作业

*/
void waitfg(pid_t pid) {
  while(pid == fgpid(jobs))
    sleep(1);
  return;
}

/*****************
 * Signal handlers
 *****************/

/*
 * sigchld_handler - The kernel sends a SIGCHLD to the shell whenever
 *     a child job terminates (becomes a zombie), or stops because it
 *     received a SIGSTOP or SIGTSTP signal. The handler reaps all
 *     available zombie children, but doesn't wait for any other
 *     currently running children to terminate.
 */
/*
  响应 停止信号 处理函数  
  回收所有僵尸进程
*/
void sigchld_handler(int sig) {
  int olderrno = errno;
  sigset_t mask_all,prev_all;
  pid_t pid;

  int status; //获取返回状态
  sigfillset(&mask_all);
  //回收所有的僵尸进程
  while((pid = waitpid(-1,&status,WNOHANG | WUNTRACED))>0){  //如果都没终止返回0 有一个被终止 返回终止pid
    if (WIFEXITED(status))  {  //正常终止
      sigprocmask(SIG_BLOCK, &mask_all, &prev_all);
      deletejob(jobs, pid);
      sigprocmask(SIG_SETMASK, &prev_all, NULL);
    }
    if(WIFSIGNALED(status))  {  //正常终止
      sigprocmask(SIG_BLOCK, &mask_all, &prev_all);
      printf("Job [%d] (%d) terminated by signal %d\n", pid2jid(pid), pid, WTERMSIG(status));
      deletejob(jobs, pid);
      sigprocmask(SIG_SETMASK, &prev_all, NULL);
    }
    if(WIFSTOPPED(status))  {  //正常暂停
      sigprocmask(SIG_BLOCK, &mask_all, &prev_all);
      printf("Job [%d] (%d) stopped by signal %d\n", pid2jid(pid), pid, WSTOPSIG(status));
      struct job_t * fgjob = getjobpid(jobs, pid);  //查找前台进程指针
      fgjob->state = ST;
      sigprocmask(SIG_SETMASK, &prev_all, NULL);
    }
  }
  errno = olderrno;  
  return;
}

/*
 * sigint_handler - The kernel sends a SIGINT to the shell whenver the
 *    user types ctrl-c at the keyboard.  Catch it and send it along
 *    to the foreground job.
 */
/*
SIGINT 信号只会传递给前台进程
*/
void sigint_handler(int sig) {
  int olderrno = errno;
  pid_t pid;
  pid = fgpid(jobs);
  kill(-pid, sig);  //发送给前台作业
  errno = olderrno;  
  return;
}

/*
 * sigtstp_handler - The kernel sends a SIGTSTP to the shell whenever
 *     the user types ctrl-z at the keyboard. Catch it and suspend the
 *     foreground job by sending it a SIGTSTP.
 */
void sigtstp_handler(int sig) {
  int olderrno = errno;
  pid_t pid;
  pid = fgpid(jobs);
  kill(-pid, sig);  //发送给前台作业
  errno = olderrno;
  return;
}

/*********************
 * End signal handlers
 *********************/




//补充函数
pid_t Fork(void) {
  pid_t pid;

  if ((pid = fork()) < 0)
    unix_error("Fork error");
  return pid;
}


void Execve(const char *filename, char *const argv[], char *const environ[]) {
  if (execve(filename, argv, environ) < 0) {
    printf("%s: Command not found.\n", filename);
    exit(0);
  }
}


/***********************************************
 * Helper routines that manipulate the job list
 **********************************************/

/* clearjob - Clear the entries in a job struct */
void clearjob(struct job_t *job) {
  job->pid = 0;
  job->jid = 0;
  job->state = UNDEF;
  job->cmdline[0] = '\0';
}

/* initjobs - Initialize the job list */
void initjobs(struct job_t *jobs) {
  int i;

  for (i = 0; i < MAXJOBS; i++)
    clearjob(&jobs[i]);
}

/* maxjid - Returns largest allocated job ID */
int maxjid(struct job_t *jobs) {
  int i, max = 0;

  for (i = 0; i < MAXJOBS; i++)
    if (jobs[i].jid > max)
      max = jobs[i].jid;
  return max;
}

/* addjob - Add a job to the job list */
int addjob(struct job_t *jobs, pid_t pid, int state, char *cmdline) {
  int i;

  if (pid < 1)
    return 0;

  for (i = 0; i < MAXJOBS; i++) {
    if (jobs[i].pid == 0) {
      jobs[i].pid = pid;
      jobs[i].state = state;
      jobs[i].jid = nextjid++;
      if (nextjid > MAXJOBS)
        nextjid = 1;
      strcpy(jobs[i].cmdline, cmdline);
      if (verbose) {
        printf("Added job [%d] %d %s\n", jobs[i].jid, jobs[i].pid, jobs[i].cmdline);
      }
      return 1;
    }
  }
  printf("Tried to create too many jobs\n");
  return 0;
}

/* deletejob - Delete a job whose PID=pid from the job list */
int deletejob(struct job_t *jobs, pid_t pid) {
  int i;

  if (pid < 1)
    return 0;

  for (i = 0; i < MAXJOBS; i++) {
    if (jobs[i].pid == pid) {
      clearjob(&jobs[i]);
      nextjid = maxjid(jobs) + 1;
      return 1;
    }
  }
  return 0;
}

/* fgpid - Return PID of current foreground job, 0 if no such job */
pid_t fgpid(struct job_t *jobs) {
  int i;

  for (i = 0; i < MAXJOBS; i++)
    if (jobs[i].state == FG)
      return jobs[i].pid;
  return 0;
}

/* getjobpid  - Find a job (by PID) on the job list */
struct job_t *getjobpid(struct job_t *jobs, pid_t pid) {
  int i;

  if (pid < 1)
    return NULL;
  for (i = 0; i < MAXJOBS; i++)
    if (jobs[i].pid == pid)
      return &jobs[i];
  return NULL;
}

/* getjobjid  - Find a job (by JID) on the job list */
struct job_t *getjobjid(struct job_t *jobs, int jid) {
  int i;

  if (jid < 1)
    return NULL;
  for (i = 0; i < MAXJOBS; i++)
    if (jobs[i].jid == jid)
      return &jobs[i];
  return NULL;
}

/* pid2jid - Map process ID to job ID */
int pid2jid(pid_t pid) {
  int i;

  if (pid < 1)
    return 0;
  for (i = 0; i < MAXJOBS; i++)
    if (jobs[i].pid == pid) {
      return jobs[i].jid;
    }
  return 0;
}

/* listjobs - Print the job list */
void listjobs(struct job_t *jobs) {
  int i;

  for (i = 0; i < MAXJOBS; i++) {
    if (jobs[i].pid != 0) {
      printf("[%d] (%d) ", jobs[i].jid, jobs[i].pid);
      switch (jobs[i].state) {
        case BG:
          printf("Running ");
          break;
        case FG:
          printf("Foreground ");
          break;
        case ST:
          printf("Stopped ");
          break;
        default:
          printf("listjobs: Internal error: job[%d].state=%d ",
                 i, jobs[i].state);
      }
      printf("%s", jobs[i].cmdline);
    }
  }
}
/******************************
 * end job list helper routines
 ******************************/


/***********************
 * Other helper routines
 ***********************/

/*
 * usage - print a help message
 */
void usage(void) {
  printf("Usage: shell [-hvp]\n");
  printf("   -h   print this message\n");
  printf("   -v   print additional diagnostic information\n");
  printf("   -p   do not emit a command prompt\n");
  exit(1);
}

/*
 * unix_error - unix-style error routine
 */
void unix_error(char *msg) {
  fprintf(stdout, "%s: %s\n", msg, strerror(errno));
  exit(1);
}

/*
 * app_error - application-style error routine
 */
void app_error(char *msg) {
  fprintf(stdout, "%s\n", msg);
  exit(1);
}

/*
 * Signal - wrapper for the sigaction function
 */
handler_t *Signal(int signum, handler_t *handler) {
  struct sigaction action, old_action;

  action.sa_handler = handler;
  sigemptyset(&action.sa_mask); /* block sigs of type being handled */
  action.sa_flags = SA_RESTART; /* restart syscalls if possible */

  if (sigaction(signum, &action, &old_action) < 0)
    unix_error("Signal error");
  return (old_action.sa_handler);
}

/*
 * sigquit_handler - The driver program can gracefully terminate the
 *    child shell by sending it a SIGQUIT signal.
 */
void sigquit_handler(int sig) {
  printf("Terminating after receipt of SIGQUIT signal\n");
  exit(1);
}

