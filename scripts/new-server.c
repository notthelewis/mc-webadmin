/*
	Used to create a new server. 
	Takes three command line arguments:-
		server type,
		server name,
		server mem
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>


/*	These are the flags used for the JVM. A valid memory value must be added before them... I.e for 5GB Of RAM: 
	java -Xms5G -Xmx5G JAVA_FLAGS    */
#define JAVA_FLAGS "-XX:+UseG1GC -XX:+ParallelRefProcEnabled -XX:MaxGCPauseMillis=200 -XX:+UnlockExperimentalVMOptions -XX:+DisableExplicitGC -XX:+AlwaysPreTouch -XX:G1NewSizePercent=30 -XX:G1MaxNewSizePercent=40 -XX:G1HeapRegionSize=8M -XX:G1ReservePercent=20 -XX:G1HeapWastePercent=5 -XX:G1MixedGCCountTarget=4 -XX:InitiatingHeapOccupancyPercent=15 -XX:G1MixedGCLiveThresholdPercent=90 -XX:G1RSetUpdatingPauseTimePercent=5 -XX:SurvivorRatio=32 -XX:+PerfDisableSharedMem -XX:MaxTenuringThreshold=1 -Dusing.aikars.flags=https://mcflags.emc.gs -Daikars.new.flags=true -jar server.jar nogui"

/* The following are the paths for the different types of server. */
#define VANILLA "../res/vanilla-1.16.4.jar"
#define SPIGOT "../res/spigot.jar"
#define PAPER "../res/paper-1.16.4-403.jar"

/*	Ensures that a valid filename is used. Legal filename can have: Letters, numbers, underscores, dashes and dots. NO spaces. */
void checkValidFileName(const char * fileName) {
	int rv, rv2; // Return values regex calculations
	regex_t allowed_name; // Regex for legal file system names
	char erbuf[100]; // Error buffer

	if (strlen(fileName) > 12) {
		printf("Filename too large! All filenames must be <= 12\n");
		exit(1);
	}

	rv = regcomp(&allowed_name, "[A-Za-z0-9_\\.\\-\\.]", 0);
	if (rv){fprintf(stderr, "unable to compile regex\n");exit(1);} // If regex doesn't compile, fail horribly.

	rv2 = regexec(&allowed_name, fileName, 0, NULL, 0);
	if (!rv2){} // Do nothing, filename allowed
	 else if (rv2 == REG_NOMATCH) { // ilegal filename ()[]$.. etc 
		printf("invalid file path. shame on you. \n");
	} else {
		regerror(rv2, &allowed_name, erbuf, sizeof(erbuf));
		fprintf(stderr, "Filename matching process broken. Consult dev: %s\n",erbuf);
		exit(1);
	}
}


int main(int argc, char const *argv[]) {
	if (argc != 4){
		printf("Usage: %s \"server_type\" \"server_name\" \"Memory to allocate\".\n\n", argv[0]);
		printf("For example\n");
		printf("%s \"vanilla\" \"test_server\" 1\n\n", argv[0]);
		printf(" Will create a vanilla server called \"test_server\" in this directory with 1GB of memory allocated\n");
		return 0;
	}	

	char command[200]; // Used to hold system commands
	char longCommand[700]; // Used to hold the bigass java command
	char * serverJar;

	const char * type = argv[1];
	const char * name = argv[2];
	      int    mem  = atoi(argv[3]);


	/* Ensures no naughty business is going on with server and file name */
	checkValidFileName(name);

	printf("Making new directory for %s server: %s\nUsing %dGB memory.\n", type, name, mem);

	/* Makes a new directory to store the server files */
	snprintf(command, sizeof(command), "mkdir %s", name);
	system(command);

	/* Agrees to EULA */
	snprintf(command, sizeof(command), "echo \"eula=true\" > %s/eula.txt", name);
	system(command);

	/* Creates a run.sh shell file inside the server folder so that the server can be started. */
	snprintf(longCommand, sizeof(longCommand), "echo \"tmux new-session -d -n %s java -Xms%dG -Xmx%dG %s\">%s/run.sh; chmod +x %s/run.sh",name,mem,mem,JAVA_FLAGS,name,name);
	system(longCommand);

	/* This server type selector could be expanded in future... For now, it's fine. */
	if(!strcmp(type, "vanilla")){
		serverJar = VANILLA;
	} else if(!strcmp(type, "spigot")){
		serverJar = SPIGOT;
	}else if(!strcmp(type, "paper")){
		serverJar = PAPER;
	}

	/* Copies the corresponding minecraft server jar file into the new folder */
	snprintf(command, sizeof(command), "cp %s %s/server.jar", serverJar, name); 
	system(command);

	snprintf(command, sizeof(command), "%s/run.sh", name);
	system(command);

	// /* Moves directory to directory specified */
	// snprintf(command, sizeof(command), "mv %s %s", name, dir);
	// system(command);

	return 0;
}