// rawfileparser.cpp
/*NOTE: ensure that the filenames are in the following format
<name>_<XxYxZ>_<format>.raw
<name> shouldn't have '_','x','.' 
Absolutely no error-checking has been implemented so be careful about the filename

The code is compliant to c++14, in Linux variants while compling with g++, add the flag -std=c++14. 
If compilation errors occur due to lack of support for c++14, modify the code accordingly.
*/

#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <bits/stdc++.h> 

 using namespace std;

/*helper function to split input filename, source: stackoverflow*/
 vector<string> split (const string &s, char delim) {
    vector<string> result;
    stringstream ss (s);
    string item;

    while (getline (ss, item, delim)) {
        result.push_back (item);
    }

    return result;
}

int main(int argc, char *argv[]){

    /*quit if the number of arguments is improper*/
	if (argc < 4) {
        std::cerr << "Usage: " << argv[0] << " <rawfile> <outputfile> <spacing>" << std::endl;
        return 1;
    }


    //parse each argument separately
    string sourceFile(argv[1]);
    string destFile(argv[2]);
    string spacing(argv[3]);    

    //split source file using '_' as delimiter to get the x,y,z extants and type of data
    vector<string> params = split(sourceFile, '_');
    
    string xyzext = params[1];
    string formatspec = params[2];

    //remove the .raw extension
    vector<string> format = split(formatspec,'.');

    //split the xyz extants using 'x' as delimiter and convert them to integers
    vector<string> xyz = split(xyzext,'x');

    int x = stoi(xyz[0]);
    int y = stoi(xyz[1]);
    int z = stoi(xyz[2]);

    //calculate the size of the data
    int size = x*y*z;

    vector <string> space = split(spacing,'x');
 
    ifstream infile; 
    ofstream outfile;

    //open the raw file to read it's contents
    infile.open(sourceFile, ios::binary | ios::in) ;

    /*ideally should have used templates instead of doing the ugly way like this :( 
    allocate a buffer of size 'size' and decide the type based on the format string
    read the data from the file to the buffer
    */
    char *datac;
    int *datai;
    if(format[0] == "uint8") {
    	datac = new char[size];
    	infile.read(datac, size);
    }
    else if(format[0] == "uint16")
    {
    	datai = new int[size];
    	infile.read((char *)datai, size);
    }
    
	
	int dp = 0;
	infile.close();


    /*write the contents of the buffer to the destination file along with other details
    x,y,z extants
    origin
    stepsize

    The output file has the data varies along these indices 
    0 0 0
    1 0 0 
    2 0 0
    ....
    ...
    _ 1 0
    ...
    ...
    ...

    _ _ 1
    ...

    x-1 y-1 z-1

    */
	outfile.open(destFile, ios::out);
	outfile << x << " " << y << " " << z << endl;
	outfile << "origin 0 0 0\n";
	outfile << "stepsize " << space[0] << " " << space[1] << " " << space[2] << endl;

	for(auto i=0; i < size; i++) {
		if(format[0] == "uint8") {
			dp = datac[i];
			if(dp < 0) dp = dp + 256;
		}	
    	else if(format[0] == "uint16") {
    		dp = datai[i] ;
    		if(dp < 0) dp = dp + 65536;
    	}
		
		outfile <<  dp << "\n";
	}
	outfile.close();

    //free the allocated buffer.
    if(format[0] == "uint8")
        delete[] datac;
    else if(format[0] == "uint16")
        delete[] datai;

    return 0;
}
