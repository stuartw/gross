// /////////////////////////////////////////////////////////////////////
// Program: BOSS
// Version: 2.1
// File:    BossClassAd.cc
// Authors: Claudio Grandi (INFN BO)
// Date:    15/07/2002
// Note:
// /////////////////////////////////////////////////////////////////////

#include <fstream>
#include <iostream>

#include "BossClassAd.h"

#include "BossOperatingSystem.h"

using namespace std;

BossClassAd::BossClassAd() {
  classad_ = new ClassAd();
}

BossClassAd::BossClassAd(string file) {
  classad_ = new ClassAd();
  readClad(file);
}

BossClassAd::BossClassAd(istream& str) {
  classad_ = new ClassAd();
  readClad(str);
}

BossClassAd::~BossClassAd() {
  delete classad_;
}

void BossClassAd::resetClad() {
  classad_->Clear();
}

int BossClassAd::readClad(istream& cladstr) {
  int return_val = 0;
  resetClad();
  if ( cladstr ) {
    // read the ClassAd
    int    line_number = 0;
    string classad_string = "";
    // Loop on all file lines
    bool hasPar = false;
    bool first = true;
    while (1) {
      int token_start = 0;
      string line;
      getline(cladstr, line, '\n');
      if (cladstr.eof())
	break;
      line_number++;
      string first_token = extract_token(&token_start, line);
      if (first_token == "" || first_token[0] == '#')
	continue;
      if ( first ) {
	if (first_token[0] == '[') {
	  hasPar = true;
	  first = false;
	} else if ( first_token[0] != ' ') {
	  first = false;
	}
      }
      // add line to classad string
      classad_string += line;
    }
    // DEBUG
    // cout << "Processed " << line_number << " lines" << endl;
    // END DEBUG

    // Add [] around ClassAd string if missing
    if ( !hasPar ) {
      classad_string = '[' + classad_string + ']';
    }
    // Parse the classad string
    ClassAdParser	parser;
    classad_ = parser.ParseClassAd(classad_string, true);
    if ( !classad_ ) {
      cerr << "BossClassAd: Bad classad, lines: " << endl 
	   << classad_string << endl;
      return_val = -3;
    }
  } else {
    cerr << "BossClassAd: Unable to read ClassAd stream" << endl;
    return_val =  -2;
  }
  // debug
  // classad_->Puke();
  // end debug
  return return_val;
}

int BossClassAd::readClad(string file) {
  int return_val = 0;
  BossOperatingSystem* sys=BossOperatingSystem::instance();
  if ( sys->fileExist(file) ) {
    // open the ClassAd file
    ifstream cladfile(file.c_str());
    if ( cladfile ) {
      return_val = readClad(cladfile);
    } else {
      cerr << "BossClassAd: Unable to open ClassAd file: " << file << endl;
      return_val = -2;
    }  
  } else {
    cerr << "BossClassAd: ClassAd file doesn't exist" << endl;
    return_val =  -1;
  }
  return return_val;
}

int BossClassAd::dumpClad(string file) {
  int return_val = 0;
  ofstream ocladfile(file.c_str());
  if ( ocladfile ) {
    dumpClad(ocladfile);
    ocladfile.close(); 
  } else {
    cerr << "BossClassAd: Unable to open output file: " << file << endl;
    return_val = -1;
  }
  return return_val;
}

int BossClassAd::dumpClad(ostream& ocladfile) {
  int return_val = 0;
  if ( ocladfile ) {
    PrettyPrint unp;
    string buffer;
    unp.Unparse( buffer, classad_ );
    if ( !buffer.empty() )
      ocladfile << buffer << endl;
  } else {
    cerr << "BossClassAd: Unable to write to output stream " << endl;
    return_val = -1;
  }
  return return_val;
}

int BossClassAd::addExpr(string key, string value) {
  int return_val = 0;
  classad_->InsertAttr(key,value);
  return return_val;
}

void BossClassAd::ClAdLookup(string key, string* value) {
  ExprTree * expression = classad_->Remove(key);
  if (expression)
    *value = expr2string(expression);
}

void BossClassAd::NestedClAdLookup(string key, vector< pair<string,string> >* value) {
  ExprTree * expression = classad_->Remove(key);
  if (expression) {
    const ExprList* elist;
    ExprListIterator itor;
    Value val;
    if ( classad_->EvaluateExpr(expression,val) ) {
      if ( val.IsListValue( elist ) ) {
	vector<ExprTree*> calist;
	elist->GetComponents(calist);
	vector<ExprTree*>::iterator it;
  	for ( it=calist.begin(); it != calist.end(); it++ ) {
	  ClassAd* tmpad;
	  if ( classad_->EvaluateExpr( (*it),val) ) {
	    if( val.IsClassAdValue( tmpad ) ) {
	      string bns = "";
	      ExprTree * bnset = tmpad->Remove("BossNamespace");
	      if (bnset) 
		bns = expr2string(bnset)+':';
	      vector< pair<string, ExprTree*> > tmpexpr;
	      tmpad->GetComponents(tmpexpr);
	      if ( tmpexpr.size()>0 ) {
		vector< pair< string, ExprTree*> >::const_iterator it1;
		for (it1=tmpexpr.begin(); it1 != tmpexpr.end(); it1++) {
		  pair<string,string> attr;
		  attr.first=bns+it1->first;
		  attr.second=expr2string(it1->second);
		  (*value).push_back(attr);
		}
	      }
	    }
	  }
	}
      }
    }
  }
}

void BossClassAd::ClAdExtractFromExpr(string key, string res, string* value) {
  ExprTree * expression = classad_->Lookup(key);
  string val = extractRequestedValue(expression,res);
  if ( !val.empty() )
    *value = val;
}

string BossClassAd::expr2string(ExprTree* expr) {
  string value="";
  if (expr) {
    PrettyPrint unp;
    unp.SetClassAdIndentation(0);
    unp.SetListIndentation(0);
    unp.SetWantStringQuotes(0);
    unp.SetMinimalParentheses(0);
    unp.Unparse( value, expr );
    value = removeOuterQuotes(value);
  }
  return value;
}

string BossClassAd::removeOuterQuotes(const string &line) {
  unsigned int i1 = 0;
  unsigned int i2 = line.size();
  while ( line[i1]==' ' )
    i1++;
  if ( line[i1] == '\"' ) {
    i1++;
    i2 = line.find_last_of("\"");
  }
  return line.substr(i1,i2-i1);
}

string BossClassAd::extract_token(int *token_start, const string &line)
{
  int token_end;
  bool in_quote;
  string token;
  
  if ((unsigned int) *token_start >= line.size()) {
    token = "";
  }
  else {
    // First, skip whitespace
    while (isspace(line[*token_start])) {
      (*token_start)++;
    }
    token_end = *token_start;
    in_quote = false;
    while ((!isspace(line[token_end]) || in_quote) && line[token_end] != 0) {
      if (line[token_end] == '\"' || line[token_end] == '\'') {
	if (token_end == *token_start) {
	  in_quote = true;
	}
	else {
	  in_quote = !in_quote;
	  token_end++;
	  break; // end quote means end of token. 
	}
      }
      // Skip over quote marks that are escaped.
      if (line[token_end] == '\\' 
	  && (line[token_end+1] == '\"' || line[token_end+1] == '\'')) {
	token_end++;
      }
      token_end++;
    }
    
    if (*token_start == token_end) {
      token = "";
    }
    else {
      token = line.substr(*token_start, token_end-(*token_start));
      *token_start = token_end + 1;
    }
  }
  return token;
}

string BossClassAd::extractRequestedValue(ExprTree* expression, const string& key) {
  string retval = "";
  if(expression) {
    string buf = expr2string(expression);
    unsigned int pos1 = buf.find_first_of(key);
    if ( pos1 >=0 && pos1 < buf.size() ) {
      unsigned int poseq = buf.find_first_of("==",pos1);
      if ( poseq >=0 && poseq < buf.size() ) {
	unsigned int posstring = buf.find_first_not_of(" ",poseq+2);
	if ( posstring >=0 && posstring < buf.size() ) {
	unsigned int pos2=posstring;
	while(pos2 < buf.size()) {
	  if (buf[pos2] == ' ' || 
	      buf[pos2] == '&' || 
	      buf[pos2] == '|' || 
	      buf[pos2] == ')' || 
	      buf[pos2] == '}' || 
	      buf[pos2] == ']' || 
	      buf[pos2] == ';' ) 
	    break;
	  pos2++;
	}
	retval = buf.substr(posstring,pos2-posstring);
	}
      }
    }
  }
  return retval;
}




