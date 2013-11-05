#include <iostream>
#include "inslookup.h"
#include <stdint.h>
#include <stdlib.h>
#include <string>
/* run this program using the console pauser or add your own getch, system("pause") or input loop */
extern char feamap_path[2048];
extern char ins_path[2048];
int main(int argc, char** argv) {
	get_param(argc, argv);
	load_feamap(feamap_path);
	char* temp = "fff142b57255f863";
	cout << strtoul(temp, NULL, 16);
	return 0;
}
