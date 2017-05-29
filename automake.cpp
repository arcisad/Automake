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
	cout << "Automatic makefile generator... By: Amir Rastar, 2016 \n\n";
	ofstream file;
	ifstream infile;
	file.open("makefile");
	file << "#makefile\n";
	file << "Comp   =   gfortran" << endl << endl;
	file << "FFLAGS  =  -g" << endl << endl;
	file << "OBJ :=  obj" << endl << endl;
	file << "VPATH :=  source" << endl << endl;
	int i=0;
	int ijk = 0;
	string *mod_names = new string[1000];
	string *mod_names_f = new string[1000];
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
				//cout << sub << endl;
				string jj;
				if (sub.compare("module") == 0 || sub.compare("program") == 0){
					size_t len = strlen(ent->d_name);
					jj = ent->d_name;
					jj = jj.substr(0,len-4);
					if (jj.compare(mname) !=0){
						mod_names[i] = ent->d_name;
						if (sub.compare("program") != 0){
							mod_names_f[ijk] = ent->d_name;
							ijk++;
						}
						cout << i << " module name: " << jj << endl;
						i++;
					}
					//cout << sub << endl << jj <<endl;
				}
			}
		}
		closedir (dir);
	} else {
	/* could not open directory */
		perror ("");
		return EXIT_FAILURE;
	}
	bool indi = true;
	do{
	indi = true;
		for (int j = 0;j <ijk; j++){
			int temk = j;
			DIR *dir3;
			struct dirent *ent3;
			if ((dir3 = opendir ("source")) != NULL) {
			/* print all the files and directories within directory */
					while ((ent3 = readdir (dir3)) != NULL) {
						string fname = ent3->d_name;
						string modname = fname;
						fname = "source/" + fname;
						string sline;
						if (modname.compare(mod_names_f[j]) == 0) {
							infile.open(fname.c_str());
							while (infile.good()){
								//for (int k = 1; k<=5; k++){
									getline(infile,sline, ',');
									istringstream ss(sline);
									do{
										string sub;
										ss >> sub;
										//cout << sub << endl;
										if (sub.compare("use") == 0 ){
											ss >> sub;
											//if (ss.peek() == ','){
												//ss.ignore();
											//}
											cout << "in " << modname << " using : " << sub << endl;
											string jj;
											for(int kk = 0;kk <i; kk++){
												size_t len = strlen(mod_names_f[kk].c_str());
												jj = mod_names_f[kk];
												jj = jj.substr(0,len-4);
												if (sub.compare(jj) == 0 && kk > temk){
													cout << "swapping " << kk << " with " << temk << endl;
													string temp;
													temp = mod_names_f[kk];
													mod_names_f[kk] = mod_names_f[temk];
													mod_names_f[temk] = temp;
													temk = kk;
													indi = false;
													//break;							
												}
											}
										}
									} while(ss);
									//cout << fname << "  " << sline << endl;
								//}
							}
							infile.close();
						}
					}
				closedir (dir3);
			} else {
			/* could not open directory */
				perror ("");
				return EXIT_FAILURE;
			}
		} 
	} while(!indi);
	string jj;
	for(int j = 0; j<ijk; j++){
		size_t len = strlen(mod_names_f[j].c_str());
		jj = mod_names_f[j];
		jj = jj.substr(0,len-4);
		file << "obj/" << jj << ".o ";
	}
	mod_names[i] = mname + ".f90";
	if (mname.compare("main") != 0){
		i++;
		mod_names[i] = "main.f90";
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
	file << "	$(Comp) ${FFLAGS} -o " << mname << " $(objects)" << endl << endl;
	file << "clean : " << endl;
	file << "	rm -f source/*.o source/*.mod" << endl;
	file << "	rm -f obj/*.o obj/*.mod" << endl;
	
	file.close();
	system("make");
	return 0;
}
