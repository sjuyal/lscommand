#include <stdio.h>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <cstring>
#include <string>
#include <vector>
#include <errno.h>
#include <algorithm>
#include <sys/ioctl.h>
#include <ctype.h>
#include <fcntl.h>
#include <time.h>
#include <pwd.h>
#include <grp.h>
#include <stdlib.h>
#define MAX_SIZE 100

#include <stdio.h>
 
#define dir "\033[1;94m"         //Blue (Directory)
#define nordper "\033[96;42m"   // Green Background and Blue Foreground (No Read Permission)
#define none   "\033[0m"  //No Colour
#define symblink "\033[1;96m"  //Cyan (Symbolic Link)
#define exec "\033[1;92m" //Green (Write permission)

using namespace std;

extern const char* const sys_errlist[];
extern int errno;
extern int sys_nerr;

//Structure of file and size information
struct fileandsize{
	char * filename;
	int size;	
};

//Structure of file and time information
struct fileandtime{
	char * filename;
	time_t t;
};

// Comparison function for comparing name of two files
bool comp(const string& var1,const string& var2){
	int k1=0,k2=0;
	char a,b;
	if(var1[0]=='.' || var1[0]=='@' || var1[0]=='_')
		k1++;
	if(var2[0]=='.' || var2[0]=='@' || var2[0]=='_')
		k2++;
	a=toupper(var1[k1]),b=toupper(var2[k2]);
	k1=0,k2=0;
	if(a<b)
		return true;
	else if (a==b){
		k1=k2=1;
		while((var1[k1]!='\0') && (var2[k2]!='\0')){
			a=toupper(var1[k1]);
			b=toupper(var2[k2]);
			k1++;
			k2++;
			if(a<b)
				return true;
			else if(a==b)
				continue;
			else
				return false;
		}	
	}
	else
		return false;	
}

// Comparison function for comparing size of two files
bool compBySize(const struct fileandsize * var1,const struct fileandsize* var2){
	if((var1->size)!=(var2->size))	
		return ((var1->size) > (var2->size));
	else{
		return comp(var1->filename,var2->filename);	
	}	
}

// Comparison function for comparing time of two files
bool compByTime(const struct fileandtime * var1,const struct fileandtime* var2){
	if((var1->t)!=(var2->t))	
		return ((var1->t) > (var2->t));
	else{
		return comp(var1->filename,var2->filename);	
	}	
}

// Finding a given option (l/a/R/S/t/d) in a vector of options
bool findopt(vector<char*> opt,char x){
	int i;
	for(i=0;i<opt.size();i++){
		string str=opt[i];
		if(str.find(x)!=-1)
			return true;	
	}
	return false;
}

// Function to sort the list of files by time
// Returns sorted vector of filenames
vector<string> sortByTime(vector<string> listfilename,const char *arg){
	struct stat *buffer;
	struct fileandtime *f;
	char *path=(char *)malloc(500*sizeof(char));  //Allocating memory to path variable (used to store filename)
	int k=0;
	vector<struct fileandtime *> temp(0);
	for(int j=0;j<listfilename.size();j++){
		// Appending directory path to filename		
		strcpy(path,arg);
		strcat(path,"/");
		strcat(path,listfilename[j].c_str());
	
		buffer=(struct stat*)malloc(sizeof(struct stat ));	
		k=stat(path,buffer);
		f=(struct fileandtime*)malloc(sizeof(struct fileandtime));   //Structure to store filename and file modif time info
		
		f->filename=(char *)malloc(100*sizeof(char));
		strcpy(f->filename,listfilename[j].c_str());
		f->t=(buffer->st_mtime);
		temp.push_back(f);         // Pushing into vector of type 'fileandtime' structure
		free(buffer);
	}
	listfilename.clear();
	sort(temp.begin(),temp.end(),compByTime);   //Sort by time function call
	for(int j=0;j<temp.size();j++){
		listfilename.push_back(temp[j]->filename);  // Pushing into vector list of filenames
	}
	return listfilename;
}

// Function to sort the list of files by size 
// Returns sorted vector of filenames
vector<string> sortBySize(vector<string> listfilename,const char *arg){
	struct stat *buffer;
	struct fileandsize *f;
	char *path=(char *)malloc(500*sizeof(char));  //Allocating memory to path variable (used to store filename)
	vector<struct fileandsize *> temp(0);

	for(int j=0;j<listfilename.size();j++){
		// Appending directory path to filename	
		strcpy(path,arg);
		strcat(path,"/");
		strcat(path,listfilename[j].c_str());	

		buffer=(struct stat*)malloc(sizeof(struct stat ));	
		stat(path,buffer);
		f=(struct fileandsize*)malloc(sizeof(struct fileandsize));  //Structure to store filename and filesize info
		
		f->filename=(char *)malloc(500*sizeof(char));
		strcpy(f->filename,listfilename[j].c_str());
		f->size=(buffer->st_size);
		temp.push_back(f);                 // Pushing into vector of type 'fileandsize' structure
		free(buffer);
	}
	listfilename.clear();
	sort(temp.begin(),temp.end(),compBySize);  // Sort By Size function call
	for(int j=0;j<temp.size();j++){
		listfilename.push_back(temp[j]->filename);  // Pushing into vector list of filenames
	}
	return listfilename;
}

//Function to print details in cases apart from '-l' option 
void printDetails(vector<string> listfilename,int cols,int maxlenoffile,bool var,const char * arg){
	int k,flag=0;
	char *path=(char *)malloc(500);     //Allocating memory to path variable (used to store filename)
	int tty=isatty(1);                  // To check whether output medium is terminal or not
	
	//if output is terminal
	if(tty){
		for(k=0;k<listfilename.size();k++){
			if(var){
        			if(listfilename[k][0]=='.')
					continue;
			}
			
			// Dividing into columns for display
			if(cols==flag){
				printf("\n");
				flag=0;
			}
			flag++;
			int sflag=0,dflag=0,rflag=1,xflag=0;
			struct stat *buffer;
	
			// Appending directory path to filename			
			strcpy(path,arg);
			strcat(path,"/");
			strcat(path,listfilename[k].c_str());	
			buffer=(struct stat*)malloc(sizeof(struct stat ));	
			stat(path,buffer);

			if(S_ISDIR(buffer->st_mode)){ dflag=1;} //Directory
			if(S_ISLNK(buffer->st_mode)){ sflag=1;} //Symbolic Link
			if(buffer->st_mode & S_IRUSR){ rflag=0;} //No Read Permission
			if(buffer->st_mode & S_IXUSR){ xflag=1;} //Executable
			// If file doesnot have read permission
			if(rflag==1){
				printf("%s",nordper);
				printf("%-*s",maxlenoffile,listfilename[k].c_str());
				printf("%s",none);			
			}			
			// If file is a directory
			else if(dflag==1){
				printf("%s",dir);
				printf("%-*s",maxlenoffile,listfilename[k].c_str());
				printf("%s",none);
			}
			// If file is a symbolic link
			else if(sflag==1){
				printf("%s",symblink);
				printf("%-*s",maxlenoffile,listfilename[k].c_str());
				printf("%s",none);
			}
			// If file is a executable
			else if(xflag==1){
				printf("%s",exec);
				printf("%-*s",maxlenoffile,listfilename[k].c_str());
				printf("%s",none);			
			}
			else {
				printf("%-*s",maxlenoffile,listfilename[k].c_str());			
			}
			free(buffer);
		}
	}
	else{
		for(k=0;k<listfilename.size();k++){
			if(var){
        			if(listfilename[k][0]=='.')
					continue;
			}
			printf("%s\n",listfilename[k].c_str());
		}	
	}
	free(path);
}

// Function to handle printing in '-l' option
void printAllDetails(vector<string> listfilename,int maxlenoffile,vector<char*> opt,const char * arg){
	struct stat *buffer;	
	char *path=(char *)malloc(500);  //Allocating memory to path variable (used to store filename)
	struct tm *stime;         //Structure to store time
	time_t t;
	struct passwd *uname;     //Structure for uname info
	struct group *grpname;    //Structure for gname info
	int maxsizeoffile=0,temp;
	char strbuf[15];
	long int totblocks=0;
	int k=0;
	int flag=0;
	int tty=isatty(1);    // storing information if output medium is terminal or not
		
	// Mark the flag if option '-a' is present
	if(flag==0){
		if(findopt(opt,'a'))
			flag=1;
	}
	
	for(int j=0;j<listfilename.size();j++){
		// Appending directory path to filename
		strcpy(path,arg);
		strcat(path,"/");
		strcat(path,listfilename[j].c_str());
		
		buffer=(struct stat*)malloc(sizeof(struct stat ));	
		k=stat(path,buffer);
		temp=buffer->st_size;
		maxsizeoffile=(temp>maxsizeoffile)?temp:maxsizeoffile;
		if(listfilename[j][0]=='.' && flag==0){  // omitting cases with '-a' option
			free(buffer);		// free the space allocated
			continue;
		}
		totblocks+=buffer->st_blocks;
		free(buffer);
	}
	
	totblocks/=2;
	printf("total %ld\n",totblocks); // Printing total blocks of 512 Mb size
	maxsizeoffile=sprintf(strbuf, "%d", maxsizeoffile);   //Finding length of file having maximum size

	for(int j=0;j<listfilename.size();j++){
		if(!findopt(opt,'a'))
			if(listfilename[j][0]=='.')       // If option is '-a' skip
				continue;
		// Appending filename to directory name
		strcpy(path,arg);
		strcat(path,"/");
		strcat(path,listfilename[j].c_str());
	
		buffer=(struct stat*)malloc(sizeof(struct stat ));	
		k=lstat(path,buffer);           // Obtaining file information

		int sflag=0,dflag=0,rflag=1,xflag=0;
               	if(k!=0)
                  	perror("File data read unsucessful");
               	else{
			if(S_ISREG(buffer->st_mode)) printf("-"); //Regular
               	        else if(S_ISDIR(buffer->st_mode)){ printf("d"); dflag=1;} //Directory
               	        else if(S_ISCHR(buffer->st_mode)) printf("c"); //Character Device Special
			else if(S_ISBLK(buffer->st_mode)) printf("b"); //Block Special
               		else if(S_ISFIFO(buffer->st_mode)) printf("p"); //Fifo (named pipe)
			else if(S_ISLNK(buffer->st_mode)){ printf("l"); sflag=1;} //Symbolic Link
			else if(S_ISSOCK(buffer->st_mode)) printf("s"); //Socket
			else printf("?");
			//Checking for file permissions
			if(buffer->st_mode & S_IRUSR){ printf("r"); rflag=0;}  // Read permission to user
				else printf("-");
			if(buffer->st_mode & S_IWUSR) printf("w");             // Write permission to user
				else printf("-");
			if(buffer->st_mode & S_IXUSR){ printf("x"); xflag=1;}  // Excecute permission to user
				else printf("-");
			if(buffer->st_mode & S_IRGRP) printf("r");             // Read permission to group
				else printf("-");
               		if(buffer->st_mode & S_IWGRP) printf("w");	       // Write permission to group
				else printf("-");
                	if(buffer->st_mode & S_IXGRP) printf("x");	       // Execute permission to group
				else printf("-");
                	if(buffer->st_mode & S_IROTH) printf("r");             // Read permission to other
               		  	else printf("-");
               	        if(buffer->st_mode & S_IWOTH) printf("w");             // Write permission to other
               		        else printf("-");;
               		if(buffer->st_mode & S_IXOTH) printf("x ");            // Excecute permission to other
                                else printf("- ");
                       	printf("%3lu ",buffer->st_nlink); //Number of hard links
			uname=getpwuid(buffer->st_uid);   //get uname from uid
			grpname=getgrgid(buffer->st_gid); //get group name from grpid
         		printf("%s ",uname->pw_name);   //Display uname
			printf("%s ",grpname->gr_name); //Display group name
               	        printf("%*ld ",maxsizeoffile,buffer->st_size); //Size in Bytes
			t = buffer->st_mtime;
			stime = localtime(&t);
			char buff[20];
			strftime (buff,80,"%h %e %H:%M ",stime);  //store time in a required format
			printf("%s",buff);  
			if(tty){            //check if output is directed to terminal
				// If file has only read permission				
				if(rflag==1){
					printf("%s",nordper);
					printf("%s",listfilename[j].c_str());
					printf("%s",none);			
				}
				// If file is a directory			
				else if(dflag==1){
					printf("%s",dir);
					printf("%-*s",maxlenoffile,listfilename[j].c_str());
					printf("%s",none);
				}
				// If file is a symbolic link
				else if(sflag==1){
					printf("%s",symblink);
					printf("%-*s",maxlenoffile,listfilename[j].c_str());
					printf("%s",none);
				}
				// If file is executable
				else if(xflag==1){
					printf("%s",exec);
					printf("%-*s",maxlenoffile,listfilename[j].c_str());
					printf("%s",none);			
				}
				else {
					printf("%-*s",maxlenoffile,listfilename[j].c_str());			
				}
			}
			// Handling link and link path
			if(sflag==1){			
				char * linkname;
				linkname =(char *) malloc(buffer->st_size + 1);  //allocating memory to linkname
				
           			int r=readlink(path, linkname, buffer->st_size + 1);
           			if(r<0){
            				exit(EXIT_FAILURE);
           			}
           			if(r>buffer->st_size){
               				fprintf(stderr, "symlink increased in size between lstat() and readlink()\n");
               				exit(EXIT_FAILURE);
           			}				

           			linkname[buffer->st_size] = '\0';
           			printf(" -> %s", linkname);
			}
           		printf("\n");
               	}
		free(buffer);	
	}
	free(path);  
}

// Function to handle if file (not directory) argument is passed
void printSingleFile(char * arg,vector<char *> opt){
	vector <string> filename;
	if(opt.size()==0)  // No option passed
		printf("%s\n",arg);
	else{
		if(findopt(opt,'l')){
			filename.push_back(arg);
			printAllDetails(filename,strlen(arg),opt,(char*)("."));	//To Display all info in case of '-l' option
		}
	}
}

//Function to print in case of '-d' option
void printSingleFileOptD(char * arg,vector<char *> opt){
	vector <string> filename;
	// prints everything in case of '-l' option 
	if(findopt(opt,'l')){
			filename.push_back(arg);
			opt.push_back((char *)"-a");
			printAllDetails(filename,strlen(arg),opt,(char*)("."));	
	}
	else{
		printf("%s",dir); // Colour for directory		
		printf("%s",arg);	
		printf("%s",none); //Reseting color
	}
}

//Function to handle '-R' (Recursive) option
void recursivelist(string arg,vector<char *> opt){
	vector<string> listfilename;      //Vector to store list of filenames
	vector<string> dirs;		 // Vector to store list of only directories
	char *path=(char *)malloc(500);	  //Allocating memory to path variable (used to store filename)
	struct stat *buffer;	        //Var of stat structure
	struct dirent **namelist;	//Var of dirent structure
	int k=0,temp,maxlenoffile=0,cols=0,winsizecol;
	errno=0; //Setting errno to zero in case if modified earlier
	if(access(arg.c_str(),X_OK)==0){
		int n = scandir(arg.c_str(), &namelist, NULL, alphasort);       //Scan the current directory
           	if(n<0)
               		perror("scandir");
           	else {
			// Storing filenames of cur directory in a vector
			// Also calculating max length of file
                	while(n--) {
				if(!(strcmp(namelist[n]->d_name,".")==0 || strcmp(namelist[n]->d_name,"..")==0)){
                			listfilename.push_back(namelist[n]->d_name);
					temp=strlen(namelist[n]->d_name);
					maxlenoffile=(temp>maxlenoffile)?temp:maxlenoffile;
				}
                   		free(namelist[n]); 
               		}
                	free(namelist);   //free the dirent structure
           	}
		// Unable to open directory
		if(errno==13){
                        printf("ls: cannot open directory %s: %s\n",arg.c_str(),sys_errlist[errno]);
			errno=0;
			return;
		}
	
		printf("\n%s:\n",arg.c_str());
		sort(listfilename.begin(),listfilename.end(),comp);
		cols=winsizecol/(maxlenoffile+1);
		// Sort by filesize		
		if(findopt(opt,'S'))
			listfilename=sortBySize(listfilename,arg.c_str());
		// Sort by file modification time
		if(findopt(opt,'T'))
			listfilename=sortByTime(listfilename,arg.c_str());

		// Print filenames of current path
		if(findopt(opt,'l')){
			printAllDetails(listfilename,maxlenoffile,opt,arg.c_str());
		}
		else {
			bool b=true;
			if(findopt(opt,'a'))
				b=false;
			printDetails(listfilename,cols,maxlenoffile,b,arg.c_str());		
		}
		

		//Recursive call inside loop
		for(int i=0;i<listfilename.size();i++){
			// Appending filename to current directory
			strcpy(path,arg.c_str());
			strcat(path,"/");
			strcat(path,listfilename[i].c_str());
			
			buffer=(struct stat*)malloc(sizeof(struct stat ));	
			k=lstat(path,buffer);        // Obtaining the file information in a structure-> stat buffer
             	  	if(k!=0)
             		    	perror("File data read unsucessful");
             	  	else{
				if(S_ISDIR(buffer->st_mode)){   // Checking if the files are directories
					if(!(strcmp(path,"./.")==0 || strcmp(path,"./..")==0)){
						recursivelist(path,opt);    // Recursively call the same function with directory name
					}
				}	
				free(buffer);    //Free the allocated buffer
			}
			
		}
	}
	free(path);  //Free the memory allocated to path
}

void listcommand(int lenoftotarg,char * argument[]){
	struct dirent *directory;       //Dirent Structure
	struct winsize tsize;		//winsize Structure
	DIR *dirstatus;
	int i=0,j=0,flag=0,temp;
	vector<char*> arg(0);		//Vector to store all the arguments
	vector<int> errfileno(0); 	//Vector to store files with errors
	vector<string> listfilename;    //Vector to stor list of filenames
	int maxlenoffile=0,winsizecol;  //Vars to store maxlength of filename and Window Column Size
	int lenoffilearg=0,lenofoptarg=0; // Temp count variables
	vector<char*> opt(0);          //Vector to store all the options
	int cols=0;

	//taking -option argument of 'ls' in a variable	
	for(i=1;i<lenoftotarg;i++){
		if(argument[i][0]=='-'){
			lenofoptarg++;
			opt.push_back(argument[i]);
		}
	}
	//putting all other arguments of 'ls' in a vector->'arg'
	for(j=1;j<lenoftotarg;j++){
		if(argument[j][0]!='-'){
			lenoffilearg++;
			arg.push_back(argument[j]);
		}
	}
	
	// Pushing all the file arguments giving error in errfileno(vector)
	for(i=0;i<arg.size();i++){
		dirstatus=opendir(arg[i]);
		if(dirstatus==NULL){
			if(errno==20)  // skipping for files which do exist but are not directories
				continue;
                        if(errno==13)  // Not able to open directory
                                printf("ls: cannot open directory %s: %s\n",arg[i],sys_errlist[errno]);
                        else if(errno==2) //Permission denied
                                printf("ls: cannot access %s: %s\n",arg[i],sys_errlist[errno]);
			errfileno.push_back(i);
                }
	}
	// Putting '.' (Current Directory) in argument if there is no argument passed by user
	if(lenoffilearg==0){
		argument[0]=(char *)(".");
		arg.push_back(argument[0]);				
	}
	// Looping for all the arguments
	for(i=0;i<arg.size();i++)
	{
		flag=0;
		// Skipping the files which show error in opening or donot exist
		for(j=0;j<errfileno.size();j++){
			if(errfileno[j]==i)
				flag=1;
		}
		if(flag==1)
			continue;

		// Open directory for current file argument
		dirstatus=opendir(arg[i]);
		if(strcmp(arg[i],(char *)".")!=0 && lenoffilearg>1 && dirstatus!=NULL)
			printf("%s:\n",arg[i]);
		maxlenoffile=0;
		
		ioctl(0,TIOCGWINSZ,&tsize);   // Puts Window size information in struct tsize
		winsizecol=tsize.ws_col;      // Obtaining cols from tsize
		listfilename.clear();

		if(dirstatus!=NULL){
			// Finding maximum length of any filename
			while((directory=readdir(dirstatus))!=NULL){
                                listfilename.push_back(directory->d_name);
				temp=strlen(directory->d_name);
				maxlenoffile=(temp>maxlenoffile)?temp:maxlenoffile;
             		}
			// Sorting the filenames
			sort(listfilename.begin(),listfilename.end(),comp);
			cols=winsizecol/(maxlenoffile+1);
			//Sorting based on Size
			if(findopt(opt,'S'))
				listfilename=sortBySize(listfilename,arg[i]);
			//Sorting based on Time
			if(findopt(opt,'t'))
				listfilename=sortByTime(listfilename,arg[i]);
		}	
		// If argument is a existing file instead of directory	
		if(errno==20){
			printSingleFile(arg[i],opt);
			errno=0;		
		}
		// Handling '-d' option
		else if(findopt(opt,'d')){       
			printSingleFileOptD(arg[i],opt);		
		}
		// Handling '-R' option
		else if(findopt(opt,'R')){
			recursivelist(arg[i],opt);		
		}
		// print information when there are no arguments
		else if(opt.size()==0){
			printDetails(listfilename,cols,maxlenoffile,true,arg[i]);
      		}	
		// Handling '-l' option
		else if(findopt(opt,'l')){
			printAllDetails(listfilename,maxlenoffile,opt,arg[i]);
		}
		// Handling '-a' option
		else if(findopt(opt,'a') && opt.size()==1 && !(findopt(opt,'S')) && !(findopt(opt,'t'))){
			printDetails(listfilename,cols,maxlenoffile,false,arg[i]);
		}
		else{
			bool b=true;
			if(findopt(opt,'a'))
				b=false;
			printDetails(listfilename,cols,maxlenoffile,b,arg[i]);		
		}
		printf("\n");		
	}
}
