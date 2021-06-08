# Deamon
 A daemon that synchronizes two subdirectories.
 <p align="right">License: GNU GPL</p>
<h3>Authors:</h3>
<p>Kamil Korenkiewicz</p> 
<p>Rafał Zaborowski</p>


<h3>List of implemented functions</h3>
Synchronization of two subdirectories every 5 minutes where the directory content is modified based on the content of the containing directory.
Ability to set the daemon sleep time using a special version of -t.
Immediately awaken the demon by sending it a SIGUSR1 source.
Sending comprehensive information to the logs about each sleep / wake action of the daemon (natural or as a result of the results).
Sending logs about errors, deletions and modification of files by the daemon.
Ability to recursively synchronize directories using a special version of -R.
Possibility to set the threshold of files, on the basis of which the method of copying will be selected. With read / write for smaller files, and when used there is a more efficient copyfile range mechanism.
When copying, the correct versions of files are also set, avoiding multiple, unnecessary copying of this file, thanks to this also changing a file in files in the target subdirectory will be undone when the daemon wakes up.

<h3>How to start the program</h3>
At the beginning, we use the command string to compile the program:

     make

Then let's start the program according to the version of the formula for using absolute paths:

    ./program -a "source_path" -b "target_path" [-R] [-s size] [-t time]

where the flags
-a -> source path with which the daemon will synchronize the subdirectory.
-b -> target path to meals that will be synchronized cyclically.
-R -> selectable, with this daemon will synchronize subdirectories recursively.
-s -> hydraulic flag, increasing the file after exceeding the value to be created / modified by the administrator copy_file_range.
-t -> optional option, setting every second the daemon should wake up, the card is the operator to wake up every 5 minutes.

Command example:

    ./program -a ~ / Desktop / PS_SYSTEMY_OPERACYJNE / Projekt / a -b ~ / Desktop / PS_SYSTEMY_OPERACYJNE / Projekt / b -R -s 2048 -t 60


<h3>Testing the program</h3>
To program test, program the script "test.sh" with the following command:

    ./test.sh

Then proceed as for "How to start the program" using analog, there are example commands, setting the subdirectory "a" as the base directory created by the script, and the subdirectory "b" as the target directory created by the script.

By running the program after framing the chunk the subdirectories will synchronize.
