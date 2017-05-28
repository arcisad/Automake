using namespace std;
#include <iostream>
#include <fstream>
#include <dirent.h>
#include <cstdlib>
#include <cstring>
#include <sstream>
#include <limits.h>


bool has_f90_extension(char const *name, char const *ext)
{
    size_t len = strlen(name);
    return len > 4 && strcmp(name + len - 4, ext) == 0;
}

int main()
{
	ofstream file;
	ifstream infile;
	file.open("makefile");
	file << "#makefile\n";
	file << "Comp   =   gfortran" << endl << endl;
	file << "FFLAGS  =  -g" << endl << endl;
	file << "OBJ :=  obj" << endl << endl;
	file << "VPATH :=  source" << endl << endl;
	int i=0;
	string *mod_names = new string[1000];;
	file << "objects = " ;
	string mname;
	cout << "Please enter your main file name: \n";
	cin >> mname;
	cin.ignore();
	string main_name = "obj/" + mname + ".o ";
		DIR *dir;
	struct dirent *ent;
	if ((dir = opendir ("source")) != NULL) {
	/* print all the files and directories within directory */
		while ((ent = readdir (dir)) != NULL) {
			string fname = ent->d_name;
			fname = "source/" + fname;
			string sline;
			if (has_f90_extension(ent->d_name,".f90")) {
				infile.open(fname.c_str());
				if (infile.good()){
					getline(infile,sline);
					//cout << fname << "  " << sline << endl;
				}
				infile.close();
				istringstream ss(sline);
				string sub;
				ss >> sub;
				cout << sub << endl;
				if (sub.compare("module") == 0 || sub.compare("program") == 0){
					mod_names[i] = ent->d_name;
					size_t len = strlen(ent->d_name);
					string jj = ent->d_name;
					jj = jj.substr(0,len-4);
					cout << sub << endl << jj <<endl;
					if (sub.compare("module") == 0) {
						file << "obj/" << jj << ".o ";
					}
					i++;
				}
			}
		}
		mod_names[i] = mname + ".f90";
		if (mname.compare("main") != 0){
			i++;
			mod_names[i] = "main.f90";
		}
		closedir (dir);
	} else {
	/* could not open directory */
		perror ("");
		return EXIT_FAILURE;
	}
	DIR *dir2;
	struct dirent *ent2;
	if ((dir2 = opendir ("source")) != NULL) {
	/* print all the files and directories within directory */
		while ((ent2 = readdir (dir2)) != NULL) {
			string fname = ent2->d_name;
			
			bool cmp = true;
			for (int j = 0;j <= i; j++){
				if (fname.compare(mod_names[j]) == 0 ){
					cmp = false;
				}
			}
			if (cmp) {
				if (has_f90_extension(ent2->d_name,".f90")) {
					size_t len = strlen(ent2->d_name);
					string jj = ent2->d_name;
					jj = jj.substr(0,len-4);
					printf ("%s\n", ent2->d_name);
					cout << jj;
					file << "obj/" << jj << ".o" << " ";
				}
			}
		}
		closedir (dir2);
	} else {
	/* could not open directory */
		perror ("");
		return EXIT_FAILURE;
	}
	file << main_name << endl << endl;
	file << "$(OBJ)/%.o : $(VPATH)/%.f90" << endl;
	file << "	$(Comp) $(FFLAGS) -c $< -o $@ -J $(OBJ)" << endl << endl;
	file << "main : $(objects)" << endl;
	file << "	$(Comp) ${FFLAGS} -o main $(objects)" << endl << endl;
	file << "clean : " << endl;
	file << "	rm -f source/*.o source/*.mod" << endl;
	file << "	rm -f obj/*.o obj/*.mod" << endl;
	
	file.close();
	system("make");
	return 0;
}
