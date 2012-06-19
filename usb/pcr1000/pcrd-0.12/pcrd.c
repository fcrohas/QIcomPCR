/*
  pcrd.c  - Unix program to control an Icom PCR-1000.
  
  Control the Icom PCR-1000 from the command line, or using
  sockets in 'daemon' mode.

  Released under GPL. Please see the file LICENSE included with
  this source code.

  This code was a derived work from the first UNIX control program
  that I found for the PCR-1000, 'pcr.c'.

  Yes, I have created a mess, and it needs lots of cleaning up now. 
  It is not bullet proof, either. I need to work on a full clean-up, and
  add some robust error checking, as well as some useful functions,
  like scanning, reporting the state of the radio over the network,
  and many other things. I just have not had the time to work all these 
  things out yet. However, what is here *is* working :-)

  I tried to get in touch with the author of the original work
  to find out how his code was licensed, but got no reply.
  There was no license information in the tar file with the 
  source code, or any reference to license on his web page.
  So, I expanded on the fine work done by sili@l0pht.com to 
  provide more control of the radio.

  Anything that does not work in this version is therefor my fault.

  I have maintained the original command line syntax to keep things
  compatible with pcr.c in this version, by just adding super-set
  commands. This will more than likely change in the future, as
  adding the commands has gotten somewhat ugly, due to lack of
  available single letter commands to implement new functions
  with abbreviations that are intuitive. Just make a mental note
  that the commands may change in the future, and any scripting
  that you do to make use of this program will more than likely have
  to change sometime. You have been forewarned. :-)

  Socket communications by default are on port 5151 of the host machine.
  Change the #define _ASSIGNED_PORT_ to any other port you wish to use
  and re-compile.

  Under Solaris, compile like this:
  cc -g -DSOLARIS -o exec_name source_name.c -lsocket -lnsl

  Under Linux, compile like this:
  cc -ggdb -DLINUX -o exec_name source_name.c

  -- or --
  
  use the Makefile! 
  make [clean] [debug_linux] [debug_solaris] [linux] [solaris] [debug_freebsd] [freebsd]

  BTW, 'Why use/write this thing?'. Well, my radio is connected to a Linux 
  server in the basement, where the antenna feedline enters the house. 
  The server has a sound card installed, and is running 'Speak_freely' for 
  UNIX (Open Source). It is running the Speak Freely 'sfreflect' reflector 
  for streaming audio, and 'sfmike' pointed at the reflector. The output 
  of the radio is connected to the line input of the sound card, and clients 
  around the home network are running both Speak Freely for UNIX and Windows, 
  so that I can listen to the radio anywhere in the house where there is a 
  computer (and that is just about everywhere, except for the bathroom 
  (for now)) connected to the ethernet. Nuff said. Geek Heaven.

  Enjoy!

  Carl Walker
  06/29/99 - 01-09-99
  v0.10
  cwalker@icmp.com
  http://www.mv.net/ipusers/cdwalker/pcrd.html

  Updated to v0.12 by Steven Haigh on 20/04/06
  http://www.crc.id.au/pcrd

  Original work: pcr.c code by
  v1.0a
  -sili@l0pht.com
  http://www3.l0pht.com/~sili/pcr.html

*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>
#include <unistd.h>
/* stuff added for socket support */
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/socket.h>
/* socket file ioctl stuff */
#include <sys/ioctl.h>
/* and signals */
#include <signal.h>

#define _ASSIGNED_PORT_ 5151; /* port to use for socket communications */

/* globals */
char not_normal_char[ 8 ];
char sig_query[ 8 ];
char pwr_query[ 8 ];
char pwron_cmd[ 8 ];
char squ_query[ 8 ];
char foffset_query[ 8 ];
char ok_reply[ 8 ];
char ng_reply[ 8 ];
char *program_name;
float version = 0.12;
void * notso_local_pointer;
pid_t daemon_pid;
time_t start_time;
time_t current_time;
time_t elapsed_time;

/* global structs */

/* This is the struc used for the alarm signal functions */
struct alarm_signal {
  int enabled;
  int expired;
  int exit_on_alarm;
  int time;
} as ;

struct radio_report {
  char squ[ 8 ];
  char sig[ 8 ];
  char foffset[ 8 ];
  char pwr[ 8 ];
  int  ok;
  int  ng;
  int count;
  char input_buffer[ 256 ];
  int report_enable;
  double last_freq;
  int last_mode;
  int last_filter;
} rpt ;

struct radio_command {
  char volcmd[ 8 ];
  char squcmd[ 8 ];
  char agccmd[ 8 ];
  char if_shiftcmd[ 8 ];
  char attncmd[ 8 ];
  char nbcmd[ 8 ];
  char off_cmd[ 8 ];
  char freq_command[ 32 ];
} cmd ;

struct command_line {
  int init_radio ,init_volume, init_squelch;
  int init_if_shift, init_attn, init_agc, init_nb;
  int default_mode, default_filter;
  int opt_d;
  int opt_off;
  int loop;
  int convert;
  int debug_level;
  char port[ 32 ];
  char default_port[ 32 ];
  char filter[ 16 ], mode[ 16 ];  
  char volume[ 16 ];
  char squelch[ 16 ];
  char if_shift[ 16 ];
  int assigned_port;
  char raw_mode[ 16 ];
  char raw_filter[ 16 ];
  int init_done;
  double freq;
  char freq_command[ 32 ];
  char freq_command_output[ 32 ];
  int daemon;
} cmd_line ;

/* Stuff needed for socket comms */
struct sockaddr_in socket_addr;
int conn;
int sock;
int conn_socket;
/* and a global for the open serial port */
int serial_fd;

/* 'one' used to fix funky Solaris call problem with socket reuse */
#ifdef SOLARIS
const char one = '1';
#endif

/* Subs */

/* The alarm signal stuff */
int alarm_set( unsigned int seconds, int enabled ) {
  int temp = 0;
  /* return the status of the alarm timer, which will be */
  /* the number of seconds until the timer will trigger */

  /* set the alarm function timer, and enabled it */
  /* if the enable int is set */
  as.enabled = enabled;
  if ( as.enabled ) {
    temp = alarm( seconds );
  }
  else {
    as.expired = 0;
    as.time = seconds;
    /* disable the alarm callback by calling alarm() with 0 */
    temp = alarm( 0 );
  }
  return temp;
}

void alarm_disable( void ) {
  /* shut off the timer / alarm signal */
  alarm_set( 0, 0 );
}

void alarm_exit_enable( void ) {
  /* make the program exit if the alarm gets triggered */
  as.exit_on_alarm = 1;
}

void alarm_exit_disable( void ) {
  /* make the program not exit if the alarm gets triggered */
  as.exit_on_alarm = 0;
}

void alarm_handler( void ) {
  int temp = 0;
  /* deal with the alarm signal when it is triggered */
  if ( as.exit_on_alarm ) {
    /* Bummer, something bad has happened, and we will bail out */
    if ( serial_fd > 0 )
      close( serial_fd );
    if ( sock > 0 )
      close( sock );
    exit ( 2 ); /* bail out */
  }
  /* Timer has triggered, but we are not going to exit */
  /* disable the alarm timer */
  alarm_set( 0, 0 );
  /* indicate that the timer has expired */
  as.expired = 1;
  /* disable the timer */
  as.enabled = 0;
}
/* END of the alarm signal stuff */

void usage( void ) {
  fprintf( stderr,
	   "Usage is %s [-i] [-s <squelch>] [-v <volume>] [-d <device>] [-C port]\n", 
	   program_name );
  fprintf( stderr,
	   "            [-A 0|1] [-N 0|1] [-S 0|1] [-I <if_shift>] <freq> <mode> <filter>\n" );
  
  fprintf( stderr,
	   "Options:\n" );
  fprintf( stderr,
	   "   -b           = Daemon Mode (Background) - I ran out of letters!\n" );
  fprintf( stderr,
	   "   -C           = Use alternate socket port nnnnn\n" );
  fprintf( stderr,
	   "   -X           = eXit (only in Daemon Mode)\n" );
  fprintf( stderr,
	   "   -D <0-9>     = Set program debugging level\n" );
  fprintf( stderr,
	   "   -R           = Suppress radio status reporting while looping\n" );
  fprintf( stderr,
	   "   -C <nnnn>    = Use this port number for socket Communications\n" );
  fprintf( stderr,
	   "   -l           = Loop sending command to radio every 5 Seconds\n" );
  fprintf( stderr,
	   "   -i           = Initialize radio\n" );
  fprintf( stderr,
	   "   -s <squelch> = Hex range from 00 to FF\n" );
  fprintf( stderr,
	   "   -v <volume>  = Hex range from 00 to FF\n" );
  fprintf( stderr,
	   "   -O           = Radio OFF\n" );
#ifdef LINUX
  fprintf( stderr,
	   "   -d <port>    = serial port. Default /dev/cua0\n" );
#else
  fprintf( stderr,
	   "   -d <port>    = serial port. Default /dev/cua/a\n" );
#endif
  fprintf( stderr,
	   "   -A <0|1>     = Automatic Gain Control <Slow | Fast>\n" );
  fprintf( stderr,
	   "   -N <0|1>     = Noise Blanker <Off | On>\n" );
  fprintf( stderr,
	   "   -S <0|1>     = RF Attenuator <Off | On>\n" );
  fprintf( stderr,
	   "   -I <00 - FF> = IF Shift 80 is Center\n" );
  fprintf( stderr,
	   "   Freq         = 0.050 -> 1300\n" );
  fprintf( stderr,
	   "   Modes        = lsb,usb,am,cw,nfm,wfm\n" );
  fprintf( stderr,
	   "   filter       = 3,6,15,50,230\n\n" );
  fprintf( stderr,
	   "Example:\n");
  fprintf( stderr,
	   "   %s 101.700 wfm 230\n", program_name ); 
  fprintf( stderr,
	   "   %s 137.500 am 50\n", program_name );
  fprintf( stderr, 
	   "\n%s looks for /dev/pcr1000 before using default port for radio connection\n",
	   program_name );
  fprintf( stderr, 
	   "\nSocket communications are assigned to port %d\n", 
	   cmd_line.assigned_port );
  fprintf( stderr, 
	   "\nThis is %s Version %1.2f\n", 
	   program_name, 
	   version );
  
  close_socket( sock );
  exit( 1 );
}

void clean_up( void ) {
  /* This is what we do when we get a signal */
  /* close the open serial port and the socket */
  if ( serial_fd > 0 )
    close( serial_fd );
  if ( sock > 0 )
    close( sock );
  exit ( 0 ); /* bail out */
}

void generate_command_set( char *argv[], int fd, int sock_mode ) {
  /* if invoked by a socket based command, make sure looping is set */
  if ( sock_mode ) 
    cmd_line.loop = 1;
  /* only work this stuff out if there is no 'off' command pending */
  if ( cmd_line.opt_off == 0 ) {
    
    /* ---- Get the command line Frequency ----  */         
    cmd_line.freq = atof( argv[ optind ] );
    /* Die if freq too big or too small */
    if ( ( cmd_line.freq > 1300 ) | ( cmd_line.freq < 0.05 ) ) {  
      if ( !sock_mode )
        usage();
      else
        return; /* blind rejection */
    }
    
    /* do the frequency conversion required for the command */
    figure_freq( (char *)&cmd_line.freq_command, 
                (char *)&cmd_line.freq_command_output, 
                cmd_line.freq );
    /* MODE */
    figure_mode( (char *)&cmd_line.raw_mode, 
                (char *)&cmd_line.mode, 
                cmd_line.default_mode, 
                cmd_line.opt_off,
		sock_mode );
    /* FILTER */
    figure_filter( (char *)&cmd_line.raw_filter, 
                  (char *)&cmd_line.filter, 
                  cmd_line.default_filter, 
                  cmd_line.opt_off,
		  sock_mode );
    /* update the reporting structure */
    rpt.last_freq = cmd_line.freq;
    strcpy( (char *)&rpt.last_mode, (char *)&cmd_line.mode );
    strcpy( (char *)&rpt.last_filter, (char *)&cmd_line.filter );

  } /* END of if(opt_off == 0) {... */

  /* check to see if we need to turn the darn thing off */
  if ( cmd_line.opt_off != 0 ) {
    strcpy( cmd.off_cmd, "H100\x0d\x0a" );
    send_command_get_reply( fd,
			    (char *)&cmd.off_cmd, 
			    (char *)&rpt.input_buffer );
    /* now that we have turned the radio off, we will set the */
    /* init flag so that the next command will */
    /* force the radio back on */
    /* (but, only if the 'off; command came from a socket connection!) */
    if ( sock_mode ) {
      cmd_line.init_radio = 1;
    }
  }
  
  /* generate a command set */
  /* first, the frequency stuff..... */
  strncpy( cmd.freq_command, "K0", 3 );
  strncat( cmd.freq_command, 
	  cmd_line.freq_command_output, 
	  strlen( cmd_line.freq_command_output ) );
  strcat( cmd.freq_command, cmd_line.mode );
  strcat( cmd.freq_command, cmd_line.filter );
  strcat( cmd.freq_command, "00" );       /* Obligatory padding */
  strcat( cmd.freq_command, "\x0d\x0a" ); /* CRLF */
  send_command_get_reply( fd, 
			  (char *)&cmd.freq_command, 
			  (char *)&rpt.input_buffer );
  
  if ( cmd_line.init_volume ) {           /* volume */
    strcpy( cmd.volcmd, "J40" );
    strcat( cmd.volcmd, cmd_line.volume );
    strcat( cmd.volcmd, "\x0d\x0a" );
    send_command_get_reply( fd, 
			    (char *)&cmd.volcmd, 
			    (char *)&rpt.input_buffer );
  }
  
  if ( cmd_line.init_squelch ) {          /* squelch */
    strcpy( cmd.squcmd, "J41" );
    strcat( cmd.squcmd, cmd_line.squelch );
    strcat( cmd.squcmd, "\x0d\x0a" );
    send_command_get_reply( fd, 
			    (char *)&cmd.squcmd, 
			    (char *)&rpt.input_buffer );
  }
  
  if ( cmd_line.init_if_shift ) {         /* IF_SHIFT */
    strcpy( cmd.if_shiftcmd, "J43" );
    strcat( cmd.if_shiftcmd, cmd_line.if_shift );
    strcat( cmd.if_shiftcmd, "\x0d\x0a" );
    send_command_get_reply( fd, 
			    (char *)&cmd.if_shiftcmd, 
			    (char *)&rpt.input_buffer );
  }
  
  /* deal with the remainder of the commands to be sent */
  if ( cmd_line.init_agc ) {
    send_command_get_reply( fd, 
			    (char *)&cmd.agccmd, 
			    (char *)&rpt.input_buffer );
  }
  
  if ( cmd_line.init_attn ) {
    send_command_get_reply( fd, 
			    (char *)&cmd.attncmd, 
			    (char *)&rpt.input_buffer );
  }
  
  if ( cmd_line.init_nb ) {
    send_command_get_reply( fd, 
			    (char *)&cmd.nbcmd, 
			    (char *)&rpt.input_buffer );
  }

}

void parse_command_line( int argc, char **argv, int sock_mode ) {
  int c;

  /* make sure that the 'off' bit is not set (only important in socket mode) */
  cmd_line.opt_off = 0; /* No longer set (if it ever was) */

  /* Get arguments from the command line */
  while ( ( c = getopt( argc, argv, "XbilORC:D:A:N:S:I:v:d:s:" ) ) != EOF )
    switch ( c ) {

    case 'X':
      if ( sock_mode ) {
	clean_up();
      }

    case 'b':
      if ( !sock_mode ) {
        cmd_line.daemon = 1;
	/* automatic set of the looping mode if we are a daemon */
        cmd_line.loop = 1; 
      }
      break;

    case 'D':
      /* set the debugging level */
      /* just bail out if the command is bad in socket mode, */
      /* exit in command line mode */
      if ( ! optarg )
        if ( sock_mode ) 
          break;
      else
        exit ( 2 );
      cmd_line.debug_level = atoi( optarg );
      if ( cmd_line.debug_level >= 10 )
        cmd_line.debug_level = 9;
      if ( cmd_line.debug_level <= 0 )
        cmd_line.debug_level = 0;
      fprintf( stdout, 
	       "Debugging Level set to %d\n", 
	       cmd_line.debug_level );
      break;

    case 'R':
      rpt.report_enable = 0; /* disable status reporting */
      break;

    case 'i':
      cmd_line.init_radio++;
      break;

    case 'v':
      if ( ! optarg )
        if ( sock_mode )
          break;
      else
        exit( 2 );
      cmd_line.init_volume++;
      sprintf( cmd_line.volume, "%s", optarg );
      break;

    case 'C':
      if ( ! optarg )
        if ( sock_mode ) 
          break;
      else
        exit( 2 );
      cmd_line.assigned_port = atoi( optarg );
      break;
      
    case 'd':
      if ( ! optarg )
        if ( sock_mode ) 
          break;
      else
        exit( 2 );
      sprintf( cmd_line.port, "%s", optarg );
      cmd_line.opt_d++;
      break;
      
    case 's':
      if ( ! optarg )
        if ( sock_mode ) 
          break;
      else
        exit( 2 );
      cmd_line.init_squelch++;
      sprintf( cmd_line.squelch, "%s", optarg );
      break;
      
    case 'A':
      if ( ! optarg )
        if ( sock_mode ) 
          break;
      else
        exit( 2 );
      cmd_line.init_agc++;
      /* see what the value of the switch is */
      cmd_line.convert = atoi( optarg );
      if ( cmd_line.convert > 0 ) { /* turn on the AGC */
        strcpy( cmd.agccmd, "J4501" );
        strcat( cmd.agccmd, "\x0d\x0a" );
      }
      else {                        /* AGC set to off  */
        strcpy( cmd.agccmd, "J4500" );           
        strcat( cmd.agccmd, "\x0d\x0a" );
      }
      break; 
      
    case 'N':
      if ( ! optarg )
        if ( sock_mode )
          break;
      else
        exit( 2 );
      cmd_line.init_nb++;
      cmd_line.convert = atoi( optarg ); 
      if ( cmd_line.convert > 0 ) { /* turn on the Noise Blanker */
        strcpy( cmd.nbcmd, "J4601" );
        strcat( cmd.nbcmd, "\x0d\x0a" );     
      }
      else { /* Noise Blanker set to off */
        strcpy( cmd.nbcmd, "J4600" );
        strcat( cmd.nbcmd, "\x0d\x0a" );
      }
      break;
      
    case 'S':
      if ( ! optarg )
        if ( sock_mode )
          break;
      else
        exit( 2 );
      cmd_line.init_attn++;
      cmd_line.convert = atoi( optarg ); 
      if ( cmd_line.convert > 0 ) { /* turn on the Attenuator */
        strcpy( cmd.attncmd, "J4701" );
        strcat( cmd.attncmd, "\x0d\x0a" );   
      }
      else { /* Attenuator set to off */
        strcpy( cmd.attncmd, "J4700" );
        strcat( cmd.attncmd, "\x0d\x0a" );
      }
      break;
      
    case 'I':
      if ( ! optarg )
        if ( sock_mode ) 
          break;
      else
        exit( 2 );
      cmd_line.init_if_shift++;
      sprintf( cmd_line.if_shift,"%s", optarg );
      break;
    case 'O':
      cmd_line.opt_off++;
      break;
    case 'l':
      cmd_line.loop++;
      break;
      
    default: /* without a valid option, display the usage stuff */
      if ( !sock_mode ) /* if not in the socket mode */
        usage();
    }
  /* We will use defaults for the mode and filter now, so we don't really */
  /* need to check them here */
  
  if ( argv[ optind + 1 ] == NULL ) { 
    cmd_line.default_mode = 1;
  }
  else {
    strcpy( cmd_line.raw_mode,argv[ optind+1 ] );
  }
  
  if ( argv[ optind + 2 ] == NULL ) { 
    cmd_line.default_filter = 1;
  }
  else {
    strcpy( cmd_line.raw_filter,argv[ optind+2 ] );
  }    
}

void report_reply_data_socket(int socket) {

  char    temp[ 80 ];
  char    mode[ 16 ];
  char  filter[ 16 ];
  char squelch[ 16 ];
  char  signal[ 16 ];
  char  offset[ 16 ];
  int        convert;

  convert = 0;

  /* Figure out what the current squelch state and frequency offset
     (if in NFM mode) are. Build ASCII strings for reporting the
     state of these items. */
  
  /* Do the signal stuff first, as it is the simplest */
  /* This is a copy of the last two chars in the string, and
     the value will be between 00 - FF */
  strncpy( (char *)&signal, ( (char *)&rpt.sig + 2 ), 3 );

  /* Now do the frequency offset */
  if ( strcmp( (char *)&rpt.foffset, "I280" ) == 0 ) {
    sprintf( (char *)&offset, "Centered" );
  }
  else if ( strcmp( (char *)&rpt.foffset, "I200" ) == 0 ) {
    sprintf( (char *)&offset, "Low" );
  }
  else if ( strcmp( (char *)&rpt.foffset, "I2FF" ) == 0 ) {
    sprintf( (char *)&offset, "High" );
  }
  else {
    sprintf( (char *)&offset, "Unknown" );
  }

  /* Now, do the squelch state */
  if ( strcmp( (char *)&rpt.squ, "I004" ) == 0 ) {
    sprintf( (char *)&squelch, "Closed " );
  }
  else if ( strcmp( (char *)&rpt.squ, "I007" ) == 0 ) {
    sprintf( (char *)&squelch, "Open " );
  }
  else {
    sprintf( (char *)&squelch, "Unknown" );    
  }

  /* print out some information on the stuff in the reply structure */
  sprintf( temp,
	   "OK Reply: %d NG Reply: %d\n", 
	   rpt.ok,
	   rpt.ng );
  write( socket, temp, strlen( temp ) );
  sprintf( temp, 
	   "ReplyCount: %d Squelch: %s ", 
	   rpt.count, 
	   (char *)&squelch );
  write( socket, temp, strlen( temp ) );
  
  /* don't print the offset if we are not in nfm mode */
  if ( ( strcmp( cmd_line.mode, "05" ) ) == 0 ) { 
    /* we are in nfm mode */
    sprintf( temp, 
	     "Signal: %s Freq. Offset: %s\n", 
	     (char *)&signal, 
	     (char *)&offset );
    write( socket, temp, strlen( temp ) );
  }
  else {
    /* we are not in nfm mode */
    sprintf( temp, 
	     "Signal: %s\n", 
	     (char *)&signal );
    write( socket, temp, strlen( temp ) );
  }
  
  /*
    Report on the mode and Filter setting, in English,
    not in the 'Radio Dribble' (Encoded) mode.
  */
  
  /* Get the mode */
  convert = atoi( (char *)&rpt.last_mode );
  /* Mode Settings:                                      */
  /* 00 == LSB                                           */
  /* 01 == USB                                           */
  /* 02 == AM                                            */
  /* 03 == CW                                            */
  /* 04 == Not used                                      */
  /* 05 == NFM                                           */
  /* 06 == WFM                                           */
  switch ( convert ) {
  case 0:
    sprintf( mode, "LSB" );
    break;
  case 1:
    sprintf( mode, "USB" );
    break;
  case 2:
    sprintf( mode, "AM" );
    break;
  case 3:
    sprintf( mode, "CW" );
    break;
  case 4:
    sprintf( mode, "Unknown" );
    break;
  case 5:
    sprintf( mode, "NFM" );
    break;
  case 6:
    sprintf( mode, "WFM" );
    break;
  default:
    sprintf( mode, "Default" );
    break;
  }
  
  /* Now, report on the filter setting, in English! */
  
  /* Filter Settings:                                    */
  /* 00 == 3 Khz (actually 2.8 Khz) (CW USB LSB AM)      */
  /* 01 == 6 Khz                    (CW USB LSB AM NFM)  */
  /* 02 == 15 Khz                   (AM NFM)             */
  /* 03 == 50 Khz                   (AM NFM WFM)         */
  /* 04 == 230 Khz                  (WFM)                */
  
  convert = atoi( (char *)&rpt.last_filter );
  
  switch ( convert ) {
  case 0:
    sprintf( filter, "2.8 KHz" );
    break;
  case 1:
    sprintf( filter, "6 KHz" );
    break;
  case 2:
    sprintf( filter, "15 KHz" );
    break;
  case 3:
    sprintf( filter, "50 KHz" );
    break;
  case 4:
    sprintf( filter, "230 KHz" );
    break;
  default:
    sprintf( filter, "Default" );
  }
  
  sprintf( temp, 
	   "Freq: %f Mode: %s Filter: %s\n", 
	   rpt.last_freq, 
	   &mode, 
	   &filter );
  write( socket, temp, strlen( temp ) );
}

int check_socket( int sock, char* greeting, char *prompt ) {
  struct sockaddr_in remote_addr;
  void * remote_addr_str;
  void * remote_addr_ptr;
  int remote_addr_len;
  int * remote_addr_len_ptr;
  int result;
  char temp[80];
  int flags;

  /* checks the socket pointed to by 'sock' and looks to see */
  /* if there is a connection returns -1 if nobody is connected, */
  /* or returns a new FD if a connection is there */

  remote_addr_ptr = &remote_addr;
  remote_addr_len = sizeof( remote_addr );
  remote_addr_len_ptr = &remote_addr_len;

  /* get the address of the connecting client in the struc remote_addr */
  result = accept( sock, remote_addr_ptr, remote_addr_len_ptr );

  if ( result < 0 ) {
    /* nobody home */
    if ( ( cmd_line.debug_level >= 6 ) & ( cmd_line.daemon == 0 ) ) {
      fprintf( stdout, 
	       "Nobody waiting to connect on socket FD %d\n", 
	       sock );
    }
    return ( -1 );
  }
  else {
    /* somebody has connected, send greeting and return the new FD */
    /* figure out what the host is from the remote_addr struct */
    remote_addr_str = inet_ntoa( remote_addr.sin_addr );
    /*    */ 
    
    if ( ( cmd_line.debug_level >= 2 ) & ( cmd_line.daemon == 0 ) ) {
      fprintf( stdout,"Client connect on socket FD %d\n", result );
    }

    /* set the newly opened (accepted) socket to non-blocking after */
    /* getting the current state of the flags */
    flags = fcntl( result, F_GETFL, 0 );
    flags |= O_NONBLOCK;
    /* set them with the O_NONBLOCK bit not set */
    fcntl( result, F_SETFL, flags );
    /* you are correct, I am not checking the return value. */
    /* It had better not fail! */

    /* send out the greeting to the connected machine */
    write( result, greeting, strlen( greeting ) );

    /* inform the user what host they are connecting from */
    sprintf( temp, "Remote host: %s\n", remote_addr_str );
    write( result, temp, strlen( temp ) );

    /* inform the user how long we have been running in daemon mode */
    sprintf( temp, "Up for %f Hours ", ( ( elapsed_time / 60.0 ) / 60.0 ) );
    write( result, temp, strlen( temp ) );
    /* Continued on the next line */
    sprintf( temp, "( %f Days)\n", ( ( elapsed_time / 60.0 ) / 60.0 ) / 24.0 );
    write( result, temp, strlen( temp ) );

    /* send out a current reciever state message to the socket now */
    report_reply_data_socket( result );

    /* and send out the command prompt */
    write( result, prompt, strlen( prompt ) );
    return( result );
  }
}

int close_socket( int sock ) {
  int flags;
  /* close the socket, if open */
  if ( sock ) {
    /* get the current state of the flags */
    flags = fcntl( sock, F_GETFL, 0 );
    flags &= ~O_NONBLOCK;
    /* set them with the O_NONBLOCK bit not set */
    if ( ( fcntl( sock, F_SETFL, flags ) ) < 0 ) {
      fprintf( stderr, 
	       "Could not set socket FD %d to Blocking: %s\n", 
	       sock, 
	       strerror( errno ) );
    }
    else if ( ( cmd_line.debug_level >= 2 ) 
	      & ( cmd_line.daemon == 0 ) ) {
      fprintf( stdout, 
	       "Socket on FD %d set to blocking operation to flush buffer.\n", 
	       sock );
    }
    /* really close it now */
    close( sock ); 

    if ( ( cmd_line.debug_level >= 2 ) 
	 & ( cmd_line.daemon == 0 ) ) {
      fprintf( stdout,
	       "Socket communications terminated on FD %d\n", 
	       sock );
    }
  }
  return 0;
}

void report_reply_data( void ) {
  /* print out some information on the stuff in the reply structure */
  if ( ( rpt.report_enable == 1 ) 
       & ( cmd_line.daemon == 0 ) ) {
    fprintf( stdout,
	     "ReplyCount: %d Squelch: %s ",
	     rpt.count, 
	     &rpt.squ );

    /* don't print the offset if we are not in nfm mode */
    if ( (strcmp( cmd_line.mode, "05" ) ) == 0 ) { 
      /* we are in nfm mode */
      fprintf( stdout,
	       "Signal: %s Offset: %s ",
	       &rpt.sig, 
	       &rpt.foffset );
    }
    else {
      /* we are not in nfm mode */
      fprintf( stdout,
	       "Signal: %s ",
	       &rpt.sig );
    }

    fprintf( stdout,
	     "Freq: %f Mode: %s Filter: %s\n", 
	     rpt.last_freq, 
	     &rpt.last_mode, 
	     &rpt.last_filter );
  }
}

void decipher_radio_reply( char *input_buffer, int len ) {
  /* bump the counter for reply */
  rpt.count++;
  /* Figure out what type of reply is in the string buffer, */
  /* and what to do about it */
  if ( match_and_update( input_buffer, 
			 (char*)&sig_query, 
			 2, 
			 (char*)&rpt.sig, 
			 4 ) )
    return;

  if ( match_and_update( input_buffer, 
			 (char*)&foffset_query, 
			 2, (char*)&rpt.foffset, 
			 4 ) )
    return;

  if ( match_and_update( input_buffer, 
			 (char*)&squ_query, 
			 2, 
			 (char*)&rpt.squ, 
			 4 ) )
    return;

  if ( match_and_update( input_buffer, 
			 (char*)&pwr_query, 
			 2, 
			 (char*)&rpt.pwr, 
			 4 ) )
    return;

  if ( match_and_update( input_buffer, 
			 (char*)&ok_reply, 
			 4, 
			 (char*)&rpt.ok, 
			 4 ) )
    return;

  if ( match_and_update( input_buffer, 
			 (char*)&ng_reply, 
			 4, 
			 (char*)&rpt.ng, 
			 4 ) )
    return;

}

int match_and_update( char *input_buffer, 
                       char *match_string, 
                       int length, 
                       char *output_location,
                       int term ) {
  /* returns 1 if the match and update worked, otherwise 0 */
  if ( strncmp( match_string, input_buffer, length ) == 0 ) {
    /* was a string match */
    /* figure out if it was a command reply (good or no_good) */
    /* or a "real" command reply that we must log... */
    if ( strncmp( ok_reply, input_buffer, length ) == 0 ) {
      rpt.ok++;
      return 1;
    }
    else if ( strncmp( ng_reply, input_buffer, length ) == 0 ) { 
      rpt.ng++;
      return 1;
    }
    /* was not either ok or ng reply, must be a real command reply */
    else {
      strncpy( output_location, input_buffer, strlen( input_buffer ) );
      /* strip the new line and other junk after the term char pos */
      output_location[ term ] = 0; 
      /* was a match, has been updated, and now let us check and make sure */
      /* than an illegal char is not in the string. If there is one in the */
      /* string, we will NULL out the contents of the string */
      if ( strpbrk( output_location, not_normal_char ) ) {
	/* It did have a 'bad' char in the string */
	*output_location = 0; 
	/* for now I will still exit with a match ok status, but I need to */
	/* think about this later */
      }
      return 1;
    }
  }
  return 0;
}

void write_check( int len_sent, int len_string ) {
  if ( ( len_sent != len_string ) 
       & ( cmd_line.daemon == 0 ) ) {
    fprintf( stderr, 
	     "Error! I wanted to send %d bytes, but was only able to send %d\n",
	     len_string, 
	     len_sent );
  }
}

int send_command_get_reply( int fd, 
			    char *sig_query, 
			    char *input_buffer ) {
  int z;
  int len;

  /* Enable the automatic alarm, so that if we do not get a reply */
  /* the daemon will exit, and not hang */
  alarm_exit_enable();
  alarm_set( 10, 1 ); /* alarm wait of 10 seconds */

  z = write( fd, sig_query, strlen( sig_query ) );
  write_check( z, ( strlen( sig_query ) ) );
  /* read data until the return length is in error (-), or less than 2 */
  len = 2;
  /* This will clear out the recieve buffer for the serial port */
  while ( len > 1 ) {
  len = ( get_radio_reply( (char *)&input_buffer, fd ) );
  }
  alarm_exit_disable(); /* disable the watchdog alarm exit bit */
  alarm_set( 0, 0 ); /* and reset the alarm timer */
  return len;
}

void restore_serial( int fd, struct termios *way_it_was ) {
  if ( ( ( tcsetattr( fd, TCSANOW, way_it_was ) ) < 0 ) 
       & ( cmd_line.daemon == 0 ) ) {
    fprintf( stderr, 
	     "restore_serial: Unable to restore port values - %s\n",
	     strerror( errno ) );
  }
}

void set_serial( int fd, 
		 struct termios *options, 
		 struct termios *way_it_was ) {
  /* Get the current options for the port... */
  tcgetattr( fd, options );
  /* save a copy of this for when we exit the program, 
  /* so that we can restore the port */
  tcgetattr( fd, way_it_was );  
  /* Set the baud rates to 9600... */
  cfsetispeed( options, B9600 );
  cfsetospeed( options, B9600 );
  /* Enable the receiver and set local mode... */
  options->c_cflag |= ( CLOCAL | CREAD );
  /* Set ICANON mode for line by lint input */
  options->c_lflag |= ( ICANON );
  /* Don't echo */
  options->c_lflag &= ~( ECHO | ECHOE );
  /* Raw output mode */
  options->c_oflag &= ~OPOST;
  /* 8N1 Settings */
  options->c_cflag &= ~PARENB;
  options->c_cflag &= ~CSTOPB;
  options->c_cflag &= ~CSIZE;
  options->c_cflag |= CS8;
  /* Don't reset the state of DTR on port close */
  options->c_cflag &= ~HUPCL;
  /* Set the new options for the port... */
  tcsetattr( fd, TCSANOW, options );
}

int open_port( char *port_name ) {
  int fd; /* File descriptor for the port */
  fd = open( port_name, O_RDWR | O_NOCTTY );
  if ( fd == -1 ) {
    /* Could not open the port */
    fprintf( stderr, "open_port: Unable to open %s - %s\n",
            port_name, strerror( errno ) );
  }
  return ( fd );
}

void close_port( int fd ) {
  close( fd );
}

int get_radio_reply( char *input_buffer, int fd ) {
  int len;
  static int reply_count;
  len = read ( fd, input_buffer, 0xff ); /* grab up to 255 bytes */
  if ( ( len < 0 ) & ( cmd_line.debug_level >= 1 ) 
       & ( cmd_line.daemon == 0 ) ) {
    fprintf( stderr, "Radio Reply Error:  %s\n", strerror( errno ) );    
  }
  if ( len > 0 ) {
    input_buffer[ ( len ) ] = 0; /* make sure there is a terminator */
  }
  /* Grab anything that the radio had to 'say' */
  /* The radio seems to send just a newline when it has nothing to say */
  if ( len > 1 ) { 
    if ( ( cmd_line.debug_level >= 1 ) 
	 & ( cmd_line.daemon == 0 ) ) {
      fprintf( stdout, 
	       "Radio Reply # %d: %s", 
	       reply_count, 
	       input_buffer );
    }
    reply_count++;
    decipher_radio_reply( input_buffer, len );
  }
  return ( len );
}

int init( int fd, int init_done, char *input_buffer ) { 
  char output[100]; 
  if ( init_done < 5 ) {

    if ( ( cmd_line.debug_level >= 1 ) & ( cmd_line.daemon == 0 ) )
      fprintf( stdout, "Sending Radio Init String\n" );

    strcpy( output, "H101\x0d\x0aG300\x0d\x0a" );   
    write( fd, output, strlen( output ) );
    init_done++;
  }
  return init_done;
}

int figure_mode(char *raw_mode, 
		char *mode, 
		int default_mode, 
		int opt_off, 
		int sock_mode) {
  /* return 1 if a mode was selected, 0 if the default mode was selected */
  if ( ( default_mode != 1 ) | ( opt_off == 0 ) ) {
    if ( ( strcmp( raw_mode, "lsb" ) ) == 0 ) {
      strcpy( mode,"00" );
    }
    else if ( ( strcmp( raw_mode, "usb" ) ) == 0 ) {
      strcpy( mode,"01" ); 
    }  
    else if ( ( strcmp( raw_mode, "am" ) ) == 0 ) {
      strcpy( mode, "02" ); 
    }
    else if ( ( strcmp( raw_mode, "cw" ) ) == 0 ) {
      strcpy( mode, "03" ); 
    }
    else if ( ( strcmp( raw_mode, "nfm" ) ) == 0 ) {
      strcpy( mode, "05" ); 
    }
    else if ( ( strcmp(raw_mode, "wfm" ) ) == 0 ) {
      strcpy( mode, "06" ); 
    }
    return 1;
  }

  else if ( sock_mode ) {
    /* just re-use the last mode in socket mode if we don't have a new one */
    return 0;
  }

  else {
    strcpy( mode, "05" );
    fprintf( stderr, "Defaulting to NFM\n" );
  }    
  return 0;
}

int figure_filter( char *raw_filter, 
		   char *filter, 
		   int default_filter, 
		   int opt_off, 
		   int sock_mode ) {
  /* return 1 if a correct filter was selected, 0 if the default filter was */
  int foo = 0;
  if ( ( default_filter != 1 ) | ( opt_off == 0 ) ) {
    foo = atoi( raw_filter );
    
    switch ( foo ) {
      
    case 3:
      strcpy( filter,"00" );
      break;
      
    case 6:
      strcpy( filter,"01" );
      break;
      
    case 15:
      strcpy( filter,"02" );
      break;
      
    case 50:
      strcpy( filter,"03" );
      break; 
      
    case 230:
      strcpy( filter,"04" );
      break;
      
    default:
      if ( cmd_line.daemon == 0 )
        fprintf( stderr,"Defaulting to 15 kHz Filter\n " );
      strcpy( filter, "02" );
      return 0;
    }
    return 1;
  }

  else if ( sock_mode ) {
    /* just re-use the last filter mode if in socket mode, */
    /* and we are not given a new one */
    return 0;
  }

  else {
    if ( cmd_line.daemon == 0 )
      fprintf( stderr," Defaulting to 6 kHz Filter\n " );
    strcpy( filter, "01" );      
    return 0;
  } 
}

int figure_freq( char *freq_command, 
		 char *freq_command_output, 
		 double freq ) {
  /* always returns 1 */
  int freq_command_output_marker = 0;
  int i = 0;
  int decimal_pos = 0;
  char temp = 0;
  /* clear out the freq_command and freq_command_output strings */
  for ( i = 0; i < 20; i++ ) {
    freq_command[ i ] = 0x0;
    freq_command_output[ i ] = 0x0;
  }
  sprintf( freq_command, "%04f", freq );
  /* This will produce a string with the freq., with a decimal point, and 6 */
  /* places of decimal. There will be a leading zero if the freq is less than */
  /* one, and no leading zero if greater than one */
  i = 0;
  /* find out where the decimal point is located */
  for ( i = 0; i <= strlen( freq_command ); i++ ) {
    temp = freq_command[ i ];
    if ( temp == '.' ) {
      /* we found it */
      decimal_pos = i;
      break;
    }
  }
  if ( decimal_pos != 0 )
    /* now, we build that section of the command string with the required */
    /* padding, now that we know where the decimal point is */
    /* figure out how much we have to pad with leading zeros */
    temp = 4 - decimal_pos;
  if ( temp > 0 ) {
    for ( i=0; i<=temp; i++ ) {
      freq_command_output[ i ] = '0';
      freq_command_output_marker = i;
    }
  }
  else {
    freq_command_output_marker = 0;
  }
  /* now we have the leading zero padding in place, we can */
  /* work in the remiander of the string */
  for ( i = 0; i < decimal_pos; i++ ) {
    freq_command_output[ freq_command_output_marker ] = freq_command[ i ];
    freq_command_output_marker++;
  }
  temp = i + 1;; /* go past the decimal point location */
  /* now we are up to the decimal point, skip that and continue writing the string */
  for ( i=temp; i < strlen(freq_command); i++ ) {
    freq_command_output[ freq_command_output_marker ] = freq_command[ i ];
    freq_command_output_marker++;
  }
  /* done building the frequency string */    
  return 1;
}

int main ( int argc, char **argv ){

  extern char *optarg;
  extern int optind;
  extern int opterr;
  
  /* --- */
struct argv_buffer {
  char cmd0[ 16 ];
  char cmd1[ 16 ];
  char cmd2[ 16 ];
  char cmd3[ 16 ];
  char cmd4[ 16 ];
  char cmd5[ 16 ];
  char cmd6[ 16 ];
  char cmd7[ 16 ];
  char cmd8[ 16 ];
  char cmd9[ 16 ];
  char cmd10[ 16 ];
  char cmd11[ 16 ];
  char cmd12[ 16 ];
  char cmd13[ 16 ];
  char cmd14[ 16 ];
  char cmd15[ 16 ];
} argv_buf;

/* pointers to the above buffers */
char * argv_buf_ptr[ 16 ];
int argc_socket;
/* --- */

  int send_command = 1; /* default is to send the command */
  /* don't forget the termios struct */
  struct termios serial_options;
  /* and one used to restore serial state on port close */
  struct termios way_it_was;

  char socket_input_buffer[ 256 ];
  int loop_count = 0;
  int z = 0;
  int i;
  int flags;
  char socket_greeting[ 80 ];
  char socket_cmd_prompt[ 80 ];
  char socket_reply_buffer[ 80 ];
  int socket_byte_count;

  /* stuff for daemon mode */
  pid_t pid;

  /*   Code Start   */
  cmd_line.daemon = 0; /* not in daemon mode by default */
  /* What time did we start running? */
  start_time = time(NULL);
  /* Zero out the good and no_good radio reply counts */
  rpt.ok = rpt.ng = 0;

  /* setup the alarm signal handling struct to the defaults */
  as.enabled = 0;
  as.expired = 0;
  as.time = 0;
  as.exit_on_alarm = 0;
  /* and, for good measure, make sure the alarm signal is off */
  alarm_disable();

#ifdef LINUX
  sprintf( cmd_line.port,"/dev/cua0" );  
  /* default to serial A on a linux box */
#else
  sprintf( cmd_line.port,"/dev/cua/a" );  
  /* default to serial A on a solaris box */
#endif
  
  /* init the pointers for the socket command line parser */
  argv_buf_ptr[ 0 ] =  (char *)&argv_buf.cmd0;
  argv_buf_ptr[ 1 ] =  (char *)&argv_buf.cmd1;
  argv_buf_ptr[ 2 ] =  (char *)&argv_buf.cmd2;
  argv_buf_ptr[ 3 ] =  (char *)&argv_buf.cmd3;
  argv_buf_ptr[ 4 ] =  (char *)&argv_buf.cmd4;
  argv_buf_ptr[ 5 ] =  (char *)&argv_buf.cmd5;
  argv_buf_ptr[ 6 ] =  (char *)&argv_buf.cmd6;
  argv_buf_ptr[ 7 ] =  (char *)&argv_buf.cmd7;
  argv_buf_ptr[ 8 ] =  (char *)&argv_buf.cmd8;
  argv_buf_ptr[ 9 ] =  (char *)&argv_buf.cmd9;
  argv_buf_ptr[ 10 ] = (char *)&argv_buf.cmd10;
  argv_buf_ptr[ 11 ] = (char *)&argv_buf.cmd11;
  argv_buf_ptr[ 12 ] = (char *)&argv_buf.cmd12;
  argv_buf_ptr[ 13 ] = (char *)&argv_buf.cmd13;
  argv_buf_ptr[ 14 ] = (char *)&argv_buf.cmd14;
  argv_buf_ptr[ 15 ] = (char *)&argv_buf.cmd15;

  /* Handle shutdown and other signals cleanly */
  signal( SIGINT,  (void *)&clean_up );
  signal( SIGTERM, (void *)&clean_up );
  signal( SIGHUP,  (void *)&clean_up );
  signal( SIGALRM, (void *)&alarm_handler );

  /* zero out the command line buffer data structure */
  memset( &argv_buf, 0, sizeof( argv_buf ) );
 
  /* clear the input buffer(s) */
  memset( &socket_input_buffer, 0, sizeof( socket_input_buffer ) );

  /* zero out the command line data structure */
  memset( &cmd_line, 0, sizeof( cmd_line ) );

  /* reset this silly counter */
  rpt.count = 0;

  program_name = argv[ 0 ];
  rpt.report_enable = 1; /* reporting enabled by default */
  cmd_line.assigned_port = _ASSIGNED_PORT_;
  /* The good and no_good command reply strings to match */
  strcpy( ok_reply, "G000" );
  strcpy( ng_reply, "G001" );
  /* power on query string */
  strcpy( pwr_query, "H1\?\x0d\x0a" );
  /* Command to keep the power on */
  strcpy( pwron_cmd, "H101\x0d\x0a" );
  /* signal strength query string */
  strcpy( sig_query, "I1\?\x0d\x0a" );
  /* squelch query string */
  strcpy( squ_query, "I0\?\x0d\x0a" );
  /* frequency offset string */
  strcpy( foffset_query, "I2\?\x0d\x0a" );
  /* and another one..... */
  sprintf( socket_cmd_prompt, "Command ->" );
  /* and a string with chars that **SHOULD NOT** be in a reply string */
  /* after stripping the cr/lf off the trailing end */
  sprintf( not_normal_char, "\x0d\x0a " );

  /* make sure that we set optind, so that we can call getopt() more */
  /* than once without a segv! */
  opterr = 0;
  optind = 1;
  /* do the normal command line parsing */
  parse_command_line( argc, argv, 0 ); 

  /* only setup the socket stuff if we are in daemon mode */
  if (cmd_line.daemon) {
  /*******************************/
  /* setup the socket stuff here */
  /*******************************/
  /* Don't bother to set the internet address to listen to */
  sock = socket( AF_INET, SOCK_STREAM, 0 );
  if (sock < 0) {
    fprintf(stderr,"Socket Open Error: %s\n",strerror(errno));
    exit ( 2 );
  }
  if ( cmd_line.debug_level >= 2 ) {
    fprintf(stdout,"Socket Open as FD: %d\n", sock);
  }

  /* allow reuse of port */
  i = 1;
#ifdef SOLARIS
  setsockopt( sock, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(i) );
#else
  setsockopt( sock, SOL_SOCKET, SO_REUSEADDR, &i, sizeof( i ) );
#endif
  /* setup this socket for non-blocking operation */
  /* get the current state of the flags */
  flags = fcntl( sock, F_GETFL, 0 );
  flags |= O_NONBLOCK;
  /* set them with the O_NONBLOCK bit set */
  if ( (fcntl( sock, F_SETFL, flags ) ) < 0 ) {
    fprintf( stderr, 
	     "Could not set socket FD %d to Non-Blocking: %s\n", 
	     sock, 
	     strerror( errno ) );
  }
  else if ( cmd_line.debug_level >= 2 ) {
    fprintf( stdout, 
	     "Socket on FD %d set to non-blocking operation\n", 
	     sock );
  }
  
  /* setup the stuff in the Internet socket address struct */
  socket_addr.sin_family = AF_INET;
  socket_addr.sin_port = htons( cmd_line.assigned_port );
  /* Zero the address field */
  memset( &socket_addr.sin_addr, 
	  0, 
	  sizeof(socket_addr.sin_addr) );

  /* bind the stuff */
  if ( bind( sock, 
	     (struct sockaddr *)&socket_addr, 
	     sizeof(socket_addr) ) != 0 ) {
    /* did not bind corretly */
    fprintf( stderr, "Socket Bind Error: %s\n", strerror( errno ) );
    exit ( 2 );
  }
  if ( listen( sock, 5 ) ) {
    fprintf( stderr, "Socket Listen Error: %s\n", strerror( errno ) );
    exit ( 2 );    
  }
  /* guess that everything worked OK! */
  if ( cmd_line.debug_level >= 2 ) {
    fprintf( stdout, "Socket communications enabled\n" );
  }
  /*********************************/
  /* end of the socket setup stuff */
  /*********************************/
  }
  
  /* Double check things to be safe.. */
  if ( (argv[optind] == NULL) 
       & (cmd_line.opt_off == 0) ) {
    /* if there is no frequency on startup, we bail out */
    fprintf(stderr,"No frequency given\n");     
    usage();
  }
  
  if ( cmd_line.opt_d == 0 ) {
    /* look for a port (a link) called 'pcr1000' first */
    /* only if -d was not used on the command line */
    sprintf( cmd_line.default_port, "/dev/pcr1000" );
    /* open it as a test */
    serial_fd = open_port( (char *)&cmd_line.default_port );
    
    if ( serial_fd < 0 ) {
      if ( cmd_line.debug_level >= 2 )
        fprintf(stderr,"Trying %s\n",cmd_line.port);
    }
    else {
      if ( cmd_line.debug_level >= 2 )
        fprintf( stdout, "Using /dev/pcr1000\n" );
      strcpy ( cmd_line.port, cmd_line.default_port );
      close_port( serial_fd );
    }
  }
  
  /* open the serial port */
  serial_fd = open_port( (char *)&cmd_line.port );
  if ( serial_fd < 0 ) {
    fprintf( stderr, "Can't open the serial port, Exiting\n" );
    close_socket( sock );
    exit( 2 );
  }
  else {
    if ( cmd_line.debug_level >= 2 )
      fprintf( stdout, "Serial port open with FD %i\n", serial_fd );
  }

  /* set the baud rate and other junk */
  set_serial( serial_fd, &serial_options, &way_it_was );
  
  send_command = 0;
  /* if looping is not enabled, we will only */
  /* send the command once and exit */
  if ( cmd_line.loop ) {
    if ( ( cmd_line.debug_level >= 2 ) & ( cmd_line.daemon == 0 ) )
      fprintf( stdout, 
	       "Sending command in loop mode. Count: %i\n", 
	       loop_count );
    loop_count++;
    send_command = 1; /* enable sending the command if loop is set */
  }

  if ( cmd_line.init_radio ) {
    cmd_line.init_done = init( serial_fd, 
			       cmd_line.init_done, 
			       (char *)&rpt.input_buffer );
    close_port( serial_fd );
    /* closing and re-opening the serial port here will */
    /* flush the buffer, and make sure that */
    /* the init string is sent out. */
    serial_fd = open_port( (char *)&cmd_line.port );
  }
 
  generate_command_set( argv, serial_fd, 0 );

  /* ***************** */
  /* Daemon mode entry */
  /* ***************** */
  /* If we have been told to be a daemon, now would */
  /* be the time to go into that mode... */
  if ( cmd_line.daemon ) {
    if ( ( pid = fork() ) < 0 ) {
      fprintf( stderr, "Whoa! Can't Fork()! Something is broken!\n" );
      exit( 2 );
    }
    else if ( pid != 0 ) {
      /* report the child PID as the daemon starts up */
      fprintf( stdout, 
	       "%s Starting in Daemon Mode as PID %d\n",
	       program_name, 
	       pid );
      exit ( 0 ); /* we are the parent, time to depart */
    }
    /* we are the child, continue living */
    setsid(); /* become the session leader */
    chdir( "/" );
    umask( 0 );
    /* we are now a 'real' daemon */
    /* find out our PID and save it for reporting */
    daemon_pid = getpid(); 
    /* a socket message with the pid embedded in it */
    sprintf( socket_greeting,
	     "Connected to pcrd Version: %1.2f PID: %d\n", 
	     version, 
	     daemon_pid );
  }

  /* Just loop requesting the signal report, */ 
  /* not sending the full commands */
  while ( send_command ) {
    if ( cmd_line.loop ) {
      /* poll the radio signal strength */
      send_command_get_reply( serial_fd, 
			      (char *)&sig_query, 
			      (char *)&rpt.input_buffer );
      /* and the squelch status */
      send_command_get_reply( serial_fd, 
			      (char *)&squ_query, 
			      (char *)&rpt.input_buffer );
      /* and the frequency offset, if in nfm mode */
      if ( strcmp( cmd_line.mode, "05" ) == 0 ) {
        send_command_get_reply( serial_fd, 
				(char *)&foffset_query, 
				(char *)&rpt.input_buffer );
      }
      report_reply_data(); /* print out the data in the structure */
      
      /* update the elapsed time since we started running */
      current_time = time(NULL);
      elapsed_time = current_time - start_time;

      /* check the socket for a connection */
      conn_socket = check_socket( sock, 
				  (char *)&socket_greeting, 
				  (char *)&socket_cmd_prompt ); 
      if ( conn_socket > 0 ) {
        socket_byte_count = -1;

        while ( socket_byte_count <= 0 ) {
          socket_byte_count = read(conn_socket, 
				   &socket_input_buffer, 
				   255); 
          if ( !( socket_byte_count <= 0 ) 
	       & ( errno == EAGAIN ) )
            break; /* break out if this is a real error */

	  /* send a power on command while we are idle, waiting for the command */
 	  send_command_get_reply( serial_fd,
  				  (char *)&pwron_cmd,
  				  (char *)&rpt.input_buffer );
	  usleep(50000); /* wait for awhile */
        }

        if ( socket_byte_count > 1 ) {
          socket_input_buffer[ socket_byte_count ] = 0; /* mark the end */ 
          /* a new line will be embedded at the end of the string from the socket */ 
          /* now we have a command line in the char string socket_input_buffer */
          /* we will break it down into whitespace seperated strings */
          {
            char * ptr;
            char tempbuf[ 256 ];
            int count = 0;
            char * local_pointer = ( char * )&socket_input_buffer;
	    /* copy the input */
            strcpy( tempbuf, socket_input_buffer );  
	    /* A dummy command in the first slot */
            strcpy( socket_input_buffer, "Socket " );
	    /* ( normally the program name ), to keep getopt() happy */
	    /* and put the stuff in the command string back */
            strcat( socket_input_buffer, tempbuf ); 
	    /* dummy value for the pointer to start the while() loop*/
            ptr = ( char * )&tempbuf;                  
            while ( ptr != NULL ) {
              ptr = ( char * )strtok( local_pointer, " " );
              if ( ptr == NULL )
                break;
              strcpy( argv_buf_ptr[ count ], ptr );
              count++;
              local_pointer = NULL;
            }
            notso_local_pointer = &argv_buf_ptr[ 0 ];
            /* parse the command we just got on the socket */
	    /* this stuff needs to be setup if we are going */
	    /* to call getopt more than one time */
            opterr = 0;
            optind = 1;
            parse_command_line( count, notso_local_pointer, 1 );
            /* fool the vars in main */
            argv = notso_local_pointer;
            argc = count;
          } /* end of local scope */
        }
	/* Now its time to generate a command set from the */
	/* command line received from the socket */
        generate_command_set( argv, serial_fd, 1 );
	/* close the socket connection */
        close_socket( conn_socket );
      }
      usleep( 500000 ); /* sleep for awhile */
    }
  } /* end of while( send_command )... */
  
  /* this part of the program is the clean up done when */ 
  /* pcrd is used as a single command */
  /* type command line program */

  /* restore the serial port the way that we found it on entry */
  restore_serial( serial_fd, &way_it_was );
  clean_up();
  if ( ( cmd_line.debug_level >= 1 ) 
       & ( cmd_line.daemon == 0 ) )
    fprintf ( stdout,"Done\n" );
  return( 0 ); /* DONE, Goodbye. */ 
}
