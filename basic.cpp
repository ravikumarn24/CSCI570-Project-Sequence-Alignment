#include<bits/stdc++.h>
#include <sys/resource.h>
#include <errno.h>
#include <stdio.h>
#include <sys/time.h>
using namespace std;

extern int errno;

int alpha[26][26];
int delta;

long getTotalMemory() {
	struct rusage usage;
	int returnCode = getrusage(RUSAGE_SELF, &usage);
	if(returnCode == 0) {
	return usage.ru_maxrss;
	} else {
	//It should never occur. Check man getrusage for more info todebug.
	// printf("error %d", errno);
	return -1;
}
}

string get_substring(string s,int start, int end){
	string substring = "";
	for(int i=start;i<end;i++){
		substring += s[i];
	}
	return substring;
}

string input_string_generator(string base_string, vector<int> indices){
	string final_string = base_string;
	for(int i=0;i<indices.size();i++){
		int index = indices[i];
		final_string = get_substring(base_string, 0, index+1) + base_string + get_substring(base_string, index+1, base_string.length());
		base_string = final_string;	
	}

	return final_string;
}

vector<string> read_from_input_file(string filepath){

	string line;
	ifstream ipfile(filepath);
	if (!ipfile.is_open()){
		exit(1);
	}
	string base_string1 = ""; 
	if(ipfile >> line){
		base_string1 = line;
	}
	vector<int> indices1;
	int value = -1;
	while(ipfile >> value){
		indices1.push_back(value);
	}
	ipfile.clear();
	string base_string2 = "";
	if(ipfile >> line){
		base_string2 = line;
	}
	value = -1;
	vector<int> indices2;
	while(ipfile >> value){
		indices2.push_back(value);
	}
	string final_string1 = input_string_generator(base_string1, indices1);
	assert(final_string1.length() == (1 << indices1.size()) * (base_string1.length()));

	string final_string2 = input_string_generator(base_string2, indices2);
	assert(final_string2.length() == (1 << indices2.size()) * (base_string2.length()));
	ipfile.close();

	return {final_string1, final_string2};
}


vector<vector<int>> basic_sequence_alignment(string s_1, string s_2){
	vector<vector<int>> dp_mat;
	for(int i=0;i<s_1.length()+1;i++){
		vector<int> row;
		for(int j=0;j<s_2.length()+1;j++){
			row.push_back(0);
		}
		dp_mat.push_back(row);
	}
	for(int i=0; i<s_2.length()+1;i++){
        	dp_mat[0][i] = i * delta;
	}
	for(int i=0; i<s_1.length()+1;i++){
		dp_mat[i][0] = i * delta;
	}
	for(int i=1; i<s_1.length()+1;i++){
		for(int j=1; j<s_2.length()+1;j++){
        		dp_mat[i][j] = min(dp_mat[i][j-1] + delta , dp_mat[i-1][j] + delta);
            		dp_mat[i][j] = min(dp_mat[i][j], dp_mat[i-1][j-1] + alpha[s_1[i-1]-'A'][s_2[j-1] - 'A']);
		}
	}
	return dp_mat;	     
}

void write_to_output_file(string filepath, string s1_aligned, string s2_aligned, int cost, long long int time_taken, long long int memory_consumed){
	ofstream opfile(filepath);
	if(!opfile.is_open()){
		exit(1);
	}
	opfile<<cost<<endl;
	opfile<<s1_aligned<<endl;
	opfile<<s2_aligned<<endl;
	opfile<<time_taken<<endl;
	opfile<<memory_consumed<<endl;
	opfile.close();

}

vector<string> get_string_alignments(vector<vector<int>> dp_mat, string s_1, string s_2){
	string s1_aligned = "";
	string s2_aligned = "";
	int r_i = s_1.length();
	int r_j = s_2.length();
	while(r_i > 0 || r_j > 0){
		if(r_i == 0 && r_j  == 0){
			break;
		}
		if(r_i == 0){
			for(int i=r_j-1;i>=0;i--){
				s1_aligned = "_" + s1_aligned;
				s2_aligned = s_2[i] + s2_aligned;
			}
			break;
		}
		if(r_j == 0){
			for(int i=r_i-1;i>=0;i--){
				s2_aligned = "_" + s2_aligned;
				s1_aligned = s_1[i] + s1_aligned;
			}
			break;
		}	

        	if(dp_mat[r_i][r_j] == dp_mat[r_i-1][r_j-1] + alpha[s_1[r_i-1]-'A'][s_2[r_j-1]-'A']){
			s1_aligned = s_1[r_i-1] + s1_aligned;
            		s2_aligned = s_2[r_j-1] + s2_aligned;
            		r_i -= 1;
            		r_j -= 1;
		}
            	else if(dp_mat[r_i][r_j] == dp_mat[r_i][r_j-1] + delta){
			s1_aligned = "_" + s1_aligned;
			s2_aligned = s_2[r_j-1] + s2_aligned;
			r_j -= 1;
		}
		else{
			s2_aligned = "_" + s2_aligned;
			s1_aligned = s_1[r_i-1] + s1_aligned;
			r_i -= 1;
		}
	}	
        return {s1_aligned,s2_aligned};
}

void verify_correctness(string s1, string s2, int cost){
	int total = 0;
	for(int i=0;i<s1.length();i++){
		if(s1[i] =='_' || s2[i] =='_'){
			total += delta;	
		}
		else{
			total += alpha[s1[i]-'A'][s2[i] - 'A'];
		}
	}
	assert(total == cost);

}

int main(int argc, char *argv[]){

	struct timeval begin, end;
	gettimeofday(&begin, 0);
	string input_filepath = argv[1];
	string output_filepath = argv[2];
	vector<string> generated_strings = read_from_input_file(input_filepath);
	delta = 30;
	map<char,map<char,int>> alpha_map = { {'A', {{'A' , 0}, {'C' , 110}, {'G', 48}, {'T', 94}} },
					  {'C', {{'A' , 110}, {'C' , 0}, {'G', 118}, {'T', 48}} },
					  {'G', {{'A' , 48}, {'C' , 118}, {'G', 0}, {'T', 110}} },
					  {'T', {{'A' , 94}, {'C' , 48}, {'G', 110}, {'T', 0}} }					
	 };
	for(int i=0;i<26;i++){
		for(int j=0;j<26;j++){
			alpha[i][j] = alpha_map[i+'A'][j+'A'];
		}
	
	}
	vector<vector<int>> dp_mat = basic_sequence_alignment(generated_strings[0], generated_strings[1]);
	vector<string> aligned_strings = get_string_alignments(dp_mat,generated_strings[0], generated_strings[1]);	

	double totalmemory = getTotalMemory();
	gettimeofday(&end, 0);
	long seconds = end.tv_sec - begin.tv_sec;
	long microseconds = end.tv_usec - begin.tv_usec;
	double totaltime = seconds*1000 + microseconds*1e-3;

	int cost = dp_mat[generated_strings[0].length()][generated_strings[1].length()];
	verify_correctness(aligned_strings[0], aligned_strings[1], cost);
	write_to_output_file(output_filepath,aligned_strings[0], aligned_strings[1], cost,totaltime,totalmemory);

	return 0;
}
