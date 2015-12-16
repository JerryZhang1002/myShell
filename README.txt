A. Finish basic function of a command shell including: running a program which can be found in either absolute path or PATH environment, redirection and pipe.

B. Known bugs: 
	a. My shell is not that robust in detecting command format errors when user inputs commands that include pipe. When using pipe, please make sure that the all the programs are valid (can be found either through the absolute path or the PATH environment variable).
	b. When using redirection, please use whitespace to separate the redirection mark and the file. The shell cannot process the redirection like "./program >output1" or "./program> output1" correctly.

C. Features:
	a. Handling invalid redirect: If the same redirection appears in the same command, the shell will accept the last redirection. E.g. For the input command like "./program > output1 > output2 < input1 < input2", shell will take output2 and input2 as the destination and source of the redirection respectively.
	b. For non-existing program, myShell just print prompt for the next command. (When not using pipe..........)

D. Testcases:
	a. I write 5 basic test programs to test my shell:
		testProgram1: output "This is testProgram1, outputing a standard output." through stdout.
		testProgram2: output "Error: this is testProgram2, outputing standard error!" through stderr.
		testProgram3: output "This is testProgram3 with exit(3)" through stdout and terminate itself by "exit(3)".
		testProgram4: output all its arguments through stdout.
		testProgram5: output all the things reading from stdin through stdout.
	b. Testcases: I input the following command in order when runing myShell in valgrind.
		(a). testcases for step1: 
		     ./testProgram1
		     ./testProgram2
		     ./testProgram3
		     ./testProgram4
		     exit(or ctrl + D)
		(b). testcases for step2:
		     ls
		     vim
		     ./testProgram4 Jianyu Zhang
		     ./testProgram4 Jianyu\ Zhang is taking ECE 551
	             non-existing-program a b c
		     exit(or ctrl + D)
	    	(c). testcases for step3:
	    	     cd ..
	    	     pushd classwork
	    	     dirstack
	    	     pushd c00
	    	     dirstack
	    	     cd ..
	    	     popd
	    	     exit(or ctrl + D)
	   	(d). testcases for step4:
	             ls | cat
	             cat < input.txt | cat
	             cat < input.txt | cat > output.txt
	             ./testProgram2 2> outputerr.txt
	   	     ls | cat | cat | cat | cat > outputdir.txt
	             exit(or ctrl + D
	    	(e). testcases for all steps:
	    	     ./testProgram1
	    	     pushd ..
	    	     ls
	    	     cd miniproject
	    	     dirstack
	    	     popd
	    	     dirstack
	    	     cd miniproject
	    	     ls | cat
	    	     cat < input.txt | cat
	    	     cat < input.txt | cat > output.txt
	    	     ./testProgram2 2> outputerr.txt
	    	     ls | cat | cat | ./testProgram5
		     exit(or ctrl + D)
	c. All testcases passed on the server and valgrind reported no memory leak.

E. 
	a. myShell.cpp
	   Contians main() function and it is the entry of myShell
	b. findProgram.cpp
	   Used for searching in the PATH environment vairable to get the full name of the program.
	c. parse.cpp
	   Used for parsing the input command.
	d. stack.cpp
	   Stack with pop, push, printStack and freeStack methods.
	e. parseinfo.cpp
	   Used for getting the info of the input command, including: whether it is piped, out_directed, err_directed, in_directed, piped program name and parameters. 
