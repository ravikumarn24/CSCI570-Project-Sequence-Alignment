#include<bits/stdc++.h>
#include <sys/resource.h>
#include <errno.h>
#include <stdio.h>
#include <sys/time.h>
using namespace std;

int alpha[26][26];
int delta;

extern int errno;

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

string get_substring(string &s,int start, int end){
	string substring = "";
	for(int i=start;i<end;i++){
		substring += s[i];
	}
	return substring;
}

string reverse_string(string &s){
	string rstring = "";
	for(int i=s.length()-1;i>=0;i--){
		rstring += s[i];
	}
	return rstring;

}

string input_string_generator(string base_string, vector<int> indices){
	string final_string = base_string;
	for(int i =0;i<indices.size();i++){
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



vector<vector<int>> sequence_alignment(string &s_1, string &s_2){
	vector<int> dp_arr;
	vector<int> dp_new_arr;

	for(int i=0;i<s_2.length()+1;i++){
		dp_arr.push_back(i * delta);
		dp_new_arr.push_back(dp_arr[i]);
	}	

	for(int i=1;i<s_1.length()+1;i++){
		dp_new_arr[0] = i * delta;
		for(int j=1; j<s_2.length()+1;j++){
			dp_new_arr[j] = min(dp_new_arr[j-1] + delta , dp_arr[j] + delta);
			dp_new_arr[j] = min(dp_new_arr[j], dp_arr[j-1] + alpha[s_1[i-1]- 'A'][s_2[j-1] - 'A']);
		}
		if(i< s_1.length()){
			dp_arr = dp_new_arr;
		}
	}

	return {dp_arr, dp_new_arr};	     
}


pair<int, vector<vector<int>>> efficient_sequence_alignment(string &s_1, string &s_2, int s1_start, int s2_start) {
	int cost = 0;
	vector<vector<int>> align_list;
	if ((s_1.empty() || s_1 == "") && ( s_2.empty() || s_2 == "")){
		return {cost, align_list};
	}

	if (s_1.empty()  || s_1 == ""){
		for(int i=0;i<s_2.length()+1;i++){
		    align_list.push_back({s1_start,s2_start + i});
		}
		return {s_2.length() * delta , align_list};
	}

	if (s_2.empty() || s_2 == ""){
		for(int i=0;i<s_1.length()+1;i++){
		    align_list.push_back({s1_start + i, s2_start});
		}
		return {s_1.length() * delta, align_list};
	}

	if (s_1.length() == 1){
		// do normal sequence alignment
		vector<vector<int>> dp_arr = sequence_alignment(s_1,s_2);
		vector<int> prev_arr1 = dp_arr[0];
		vector<int> arr1 = dp_arr[1];
		int i = 1;
		int j = arr1.size()-1;
		while(i >= 0 || j >= 0){
			if(i == 1){
				if(j > 0 && (arr1[j] == arr1[j-1] + delta)){
					align_list.push_back({s1_start+i,s2_start+j});
					j-=1;
				}
				else if(arr1[j] == prev_arr1[j] + delta){
					align_list.push_back({s1_start + i, s2_start + j});
					i -= 1;
				}
				else{
					align_list.push_back({s1_start + i, s2_start + j});
					i -= 1;
					j -= 1;
				}
			}
			else{
				align_list.push_back({s1_start + i, s2_start + j});
				j -= 1;
				if(j < 0){
					i = -1;
				}
			}
		}
		vector<vector<int>> reversed_align_list;
		for(int ii=align_list.size()-1;ii>=0;ii--){
			reversed_align_list.push_back(align_list[ii]);
		}
		return {arr1[arr1.size()-1], reversed_align_list};
	}

	int s1_split = s_1.length() / 2;
	string s1_first = get_substring(s_1, 0,s1_split);
	string s1_second = get_substring(s_1,s1_split,s_1.length());



	vector<vector<int>> dp_mat1 = sequence_alignment(s1_first, s_2);
	vector<int> prev_arr1 = dp_mat1[0];
	vector<int> arr1 = dp_mat1[1];

	string s1_second_reverse = reverse_string(s1_second);
	string s2_reverse = reverse_string(s_2);
	vector<vector<int>> dp_mat2 = sequence_alignment(s1_second_reverse, s2_reverse);
	vector<int> prev_arr2 = dp_mat2[0];
	vector<int> arr2 = dp_mat2[1];
	

	int s2_split = -1;
	int min_val = -1;
	
	for(int i=0; i<arr1.size();i++){
		if(min_val == -1){
			min_val = arr1[i] + arr2[arr2.size()-(i+1)];
			s2_split = i;
		}
		else if(min_val > arr1[i] + arr2[arr2.size()-(i+1)]){
			min_val = arr1[i] + arr2[arr2.size()-(i+1)];
			s2_split = i;
		}

	}
	
	
	string s2_first = get_substring(s_2, 0, s2_split);
	string s2_second = get_substring(s_2, s2_split, s_2.length());
	
	
	pair<int,vector<vector<int>>> leftPair = efficient_sequence_alignment(s1_first, s2_first, s1_start, s2_start);
	pair<int, vector<vector<int>>> rightPair = efficient_sequence_alignment(s1_second, s2_second, s1_start + s1_split , s2_start + s2_split);
	
	for(int i=0;i<leftPair.second.size();i++){
		align_list.push_back(leftPair.second[i]);
	}
	
	align_list.push_back({s1_start + s1_split, s2_start + s2_split});
	
	for(int i=0;i<rightPair.second.size();i++){
		align_list.push_back(rightPair.second[i]);
	}

	return {min_val , align_list};

	
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

vector<string> get_string_alignments(vector<vector<int>> &align_list, string &final_string1, string &final_string2){
	string s1_aligned = "";
	string s2_aligned = "";
	int i = 0;
	int j = 0;
	for(int itr = 0; itr<align_list.size();itr++){
		vector<int> coord = align_list[itr];
		int a = coord[0];
		int b = coord[1];
		//cout<<"("<<a<<" "<<b<<") , ";
		if(a == i && b == j){
			continue;
		}
		int prev_i = i;
		int prev_j = j;
		while(i <= a-1){
		    s1_aligned += final_string1[i];
		    i += 1;
		    if(b == prev_j){
			s2_aligned += '_';
		    } 
		}
		       
		while(j <= b-1){
		    s2_aligned += final_string2[j];
		    j += 1;
		    if(a == prev_i){
			s1_aligned += '_';
		    }
		}
	}

    return {s1_aligned, s2_aligned};	
}

void verify_correctness(string s1, string s2, int cost){
	int total = 0;
	for(int i=0;i<s1.length();i++){
		if(s1[i] =='_' || s2[i] =='_'){
			total += delta;	
		}
		else{
			total += alpha[s1[i]-'A'][s2[i]-'A'];
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

	pair<int, vector<vector<int>>> cost_list_pair = efficient_sequence_alignment(generated_strings[0], generated_strings[1],0,0);
	vector<string> aligned_strings = get_string_alignments(cost_list_pair.second,generated_strings[0], generated_strings[1]);	

	double totalmemory = getTotalMemory();
	gettimeofday(&end, 0);
	long seconds = end.tv_sec - begin.tv_sec;
	long microseconds = end.tv_usec - begin.tv_usec;
	double totaltime = seconds*1000 + microseconds*1e-3;

	int cost = cost_list_pair.first;
	verify_correctness(aligned_strings[0], aligned_strings[1], cost);	
	write_to_output_file(output_filepath,aligned_strings[0], aligned_strings[1], cost,totaltime,totalmemory);
	return 0;
}
