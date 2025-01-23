# vnode_kqueue
Using kqueue to monitor events on file operations.
## Intro
Kernel event queue is a fast signal-delivery mechanism which allows a program to process events from operating system in a very fast and effective way. The kernel queue is a bunch of ADT living in kernel memory and a bunch of kernel code that operates with this data to notify a user-level application about various system events. A user app can't access this data directly (it's managed by kernel) and so it operates with kernel queue - or kqueue - via the API that operating system provides. 

In this short demo code I'll try to demonstrate the capability of kqueue by utilizing to monitor filesystem operations on a given file.

## HOW TO BUILD
```
  % make depend && make 
```

## SYNOPSYS
The ***filemonitor*** application opens a file - the parameter after the application name -, and monitors various events occur on the filedescriptor.

## USAGE
```
  % ./filemonitor <existing filename>
```
In the example I attached a file - called data.txt -, this file will be used to catch various events during manipulating the opened file.
## DEMO RUN
```
  %./filemonitor data.txt

```
The application opens the file and waits for events.
### Display the content of the file
(*Open a second terminal to be able to manipulate the file or it's contents!*)
In the second terminal type the following command:
```
  % cat data.txt
  1012
```
The first output of the terminal where we executed the program goes like this:
```
  %./filemonitor data.txt
  Event 4 occurred.  Filter -4, flags 32, filter flags NOTE_OPEN, filter data 0, path data.txt
  Event 4 occurred.  Filter -4, flags 32, filter flags NOTE_CLOSE, filter data 0, path data.txt
```
### Overwrite the content of the file
```
  % echo 1013 > data.txt
```
The output will be this:
```
  %./filemonitor data.txt
  Event 4 occurred.  Filter -4, flags 32, filter flags NOTE_OPEN, filter data 0, path data.txt
  Event 4 occurred.  Filter -4, flags 32, filter flags NOTE_CLOSE, filter data 0, path data.txt
  Event 4 occurred.  Filter -4, flags 32, filter flags NOTE_ATTRIB|NOTE_OPEN, filter data 0, path data.txt
  Event 4 occurred.  Filter -4, flags 32, filter flags NOTE_WRITE|NOTE_EXTEND, filter data 0, path data.txt
  Event 4 occurred.  Filter -4, flags 32, filter flags NOTE_CLOSE_WRITE, filter data 0, path data.txt
```
### Rename the original file
```
  % mv data.txt test.txt
```
The output will be this:
```
  %./filemonitor data.txt
  Event 4 occurred.  Filter -4, flags 32, filter flags NOTE_OPEN, filter data 0, path data.txt
  Event 4 occurred.  Filter -4, flags 32, filter flags NOTE_CLOSE, filter data 0, path data.txt
  Event 4 occurred.  Filter -4, flags 32, filter flags NOTE_ATTRIB|NOTE_OPEN, filter data 0, path data.txt
  Event 4 occurred.  Filter -4, flags 32, filter flags NOTE_WRITE|NOTE_EXTEND, filter data 0, path data.txt
  Event 4 occurred.  Filter -4, flags 32, filter flags NOTE_CLOSE_WRITE, filter data 0, path data.txt
  Event 4 occurred.  Filter -4, flags 32, filter flags NOTE_RENAME, filter data 0, path data.txt
```
### Get the content of the renamed file - note, the original filename remains the same as it was defined at startup
```
  % cat test.txt
  1013
```
The output will be this:
```
  %./filemonitor data.txt
  Event 4 occurred.  Filter -4, flags 32, filter flags NOTE_OPEN, filter data 0, path data.txt
  Event 4 occurred.  Filter -4, flags 32, filter flags NOTE_CLOSE, filter data 0, path data.txt
  Event 4 occurred.  Filter -4, flags 32, filter flags NOTE_ATTRIB|NOTE_OPEN, filter data 0, path data.txt
  Event 4 occurred.  Filter -4, flags 32, filter flags NOTE_WRITE|NOTE_EXTEND, filter data 0, path data.txt
  Event 4 occurred.  Filter -4, flags 32, filter flags NOTE_CLOSE_WRITE, filter data 0, path data.txt
  Event 4 occurred.  Filter -4, flags 32, filter flags NOTE_RENAME, filter data 0, path data.txt
  Event 4 occurred.  Filter -4, flags 32, filter flags NOTE_OPEN, filter data 0, path data.txt
  Event 4 occurred.  Filter -4, flags 32, filter flags NOTE_CLOSE, filter data 0, path data.txt
```
### Making a copy of the original file using its original name
```
  % cp -p test.txt data.txt
```
The output will be this:
```
  %./filemonitor data.txt
  Event 4 occurred.  Filter -4, flags 32, filter flags NOTE_OPEN, filter data 0, path data.txt
  Event 4 occurred.  Filter -4, flags 32, filter flags NOTE_CLOSE, filter data 0, path data.txt
  Event 4 occurred.  Filter -4, flags 32, filter flags NOTE_ATTRIB|NOTE_OPEN, filter data 0, path data.txt
  Event 4 occurred.  Filter -4, flags 32, filter flags NOTE_WRITE|NOTE_EXTEND, filter data 0, path data.txt
  Event 4 occurred.  Filter -4, flags 32, filter flags NOTE_CLOSE_WRITE, filter data 0, path data.txt
  Event 4 occurred.  Filter -4, flags 32, filter flags NOTE_RENAME, filter data 0, path data.txt
  Event 4 occurred.  Filter -4, flags 32, filter flags NOTE_OPEN, filter data 0, path data.txt
  Event 4 occurred.  Filter -4, flags 32, filter flags NOTE_CLOSE, filter data 0, path data.txt
  Event 4 occurred.  Filter -4, flags 32, filter flags NOTE_OPEN, filter data 0, path data.txt
  Event 4 occurred.  Filter -4, flags 32, filter flags NOTE_CLOSE, filter data 0, path data.txt
```
### Finally remove the file - the renamed file's descriptor is the same as the one which was obtained at open(2).
```
  % rm test.txt
```
The output will be this (note that the event triggered by removing '*test.txt*'!):
```
  %./filemonitor data.txt
  Event 4 occurred.  Filter -4, flags 32, filter flags NOTE_OPEN, filter data 0, path data.txt
  Event 4 occurred.  Filter -4, flags 32, filter flags NOTE_CLOSE, filter data 0, path data.txt
  Event 4 occurred.  Filter -4, flags 32, filter flags NOTE_ATTRIB|NOTE_OPEN, filter data 0, path data.txt
  Event 4 occurred.  Filter -4, flags 32, filter flags NOTE_WRITE|NOTE_EXTEND, filter data 0, path data.txt
  Event 4 occurred.  Filter -4, flags 32, filter flags NOTE_CLOSE_WRITE, filter data 0, path data.txt
  Event 4 occurred.  Filter -4, flags 32, filter flags NOTE_RENAME, filter data 0, path data.txt
  Event 4 occurred.  Filter -4, flags 32, filter flags NOTE_OPEN, filter data 0, path data.txt
  Event 4 occurred.  Filter -4, flags 32, filter flags NOTE_CLOSE, filter data 0, path data.txt
  Event 4 occurred.  Filter -4, flags 32, filter flags NOTE_OPEN, filter data 0, path data.txt
  Event 4 occurred.  Filter -4, flags 32, filter flags NOTE_CLOSE, filter data 0, path data.txt
  Event 4 occurred.  Filter -4, flags 32, filter flags NOTE_DELETE, filter data 0, path data.txt
```
### Check to see that we do not capture event when playing with the copy of the removed file.
```
  % cat data.txt
  1013
```
The output will be the same as before:
```
  %./filemonitor data.txt
  Event 4 occurred.  Filter -4, flags 32, filter flags NOTE_OPEN, filter data 0, path data.txt
  Event 4 occurred.  Filter -4, flags 32, filter flags NOTE_CLOSE, filter data 0, path data.txt
  Event 4 occurred.  Filter -4, flags 32, filter flags NOTE_ATTRIB|NOTE_OPEN, filter data 0, path data.txt
  Event 4 occurred.  Filter -4, flags 32, filter flags NOTE_WRITE|NOTE_EXTEND, filter data 0, path data.txt
  Event 4 occurred.  Filter -4, flags 32, filter flags NOTE_CLOSE_WRITE, filter data 0, path data.txt
  Event 4 occurred.  Filter -4, flags 32, filter flags NOTE_RENAME, filter data 0, path data.txt
  Event 4 occurred.  Filter -4, flags 32, filter flags NOTE_OPEN, filter data 0, path data.txt
  Event 4 occurred.  Filter -4, flags 32, filter flags NOTE_CLOSE, filter data 0, path data.txt
  Event 4 occurred.  Filter -4, flags 32, filter flags NOTE_OPEN, filter data 0, path data.txt
  Event 4 occurred.  Filter -4, flags 32, filter flags NOTE_CLOSE, filter data 0, path data.txt
  Event 4 occurred.  Filter -4, flags 32, filter flags NOTE_DELETE, filter data 0, path data.txt
```
### Exit from monitor application: press Ctrl+C
```
  %./filemonitor data.txt
  Event 4 occurred.  Filter -4, flags 32, filter flags NOTE_OPEN, filter data 0, path data.txt
  Event 4 occurred.  Filter -4, flags 32, filter flags NOTE_CLOSE, filter data 0, path data.txt
  Event 4 occurred.  Filter -4, flags 32, filter flags NOTE_ATTRIB|NOTE_OPEN, filter data 0, path data.txt
  Event 4 occurred.  Filter -4, flags 32, filter flags NOTE_WRITE|NOTE_EXTEND, filter data 0, path data.txt
  Event 4 occurred.  Filter -4, flags 32, filter flags NOTE_CLOSE_WRITE, filter data 0, path data.txt
  Event 4 occurred.  Filter -4, flags 32, filter flags NOTE_RENAME, filter data 0, path data.txt
  Event 4 occurred.  Filter -4, flags 32, filter flags NOTE_OPEN, filter data 0, path data.txt
  Event 4 occurred.  Filter -4, flags 32, filter flags NOTE_CLOSE, filter data 0, path data.txt
  Event 4 occurred.  Filter -4, flags 32, filter flags NOTE_OPEN, filter data 0, path data.txt
  Event 4 occurred.  Filter -4, flags 32, filter flags NOTE_CLOSE, filter data 0, path data.txt
  Event 4 occurred.  Filter -4, flags 32, filter flags NOTE_DELETE, filter data 0, path data.txt
  ^CReceived UNIX signal via kqueue: 2
  [I] Closing monitoring descriptor 4
  [I] Closing kqueue handler 3
  Exiting...
```
