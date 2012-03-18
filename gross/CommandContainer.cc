#include "CommandContainer.hh"

#include "Prepare.hh"
#include "Submit.hh"
#include "Register.hh"
#include "Query.hh"
#include "Output.hh"
#include "Delete.hh"

CommandContainer::CommandContainer() {
  // All the command interfaces are created here
  // allow aliases for commands
  Command* c;
  // Prepare
  c=new Prepare();
  cmd_["prepare"]=c;
  cmd_["p"]=c;
  //Submit
  c=new Submit();
  cmd_["submit"]=c;
  cmd_["s"]=c;
  //Status
  c=new Query();
  cmd_["query"]=c;
  cmd_["q"]=c;
  //Output files
  c=new Output();
  cmd_["output"]=c;
  cmd_["o"]=c;
  //Delete
  c=new Delete();
  cmd_["delete"]=c;
  cmd_["d"]=c;
  //Register Wrapper
  c=new Register();
  cmd_["register"]=c;
  cmd_["r"]=c;
}

CommandContainer::~CommandContainer() {
  /*
  for(Commands_iterator i=cmd_.begin();i!=cmd_.end();i++) {
    if(i->second){ // do not delete twice the same command (allow for aliases)
      delete i->second;
      i->second=0;
    }
  }
  */
}

Command* CommandContainer::command(const string s) const {
  Commands_const_iterator ci = cmd_.find(s);
  if(ci==cmd_.end())
    return 0;
  else
    return ci->second;
}

void CommandContainer::printUsage() const 
{
  cout << "Usage:" << endl << "gross command [command options]" << endl;
  cout << "command:" << endl
       << "\tprepare [p]          : prepare analysis task" << endl
       << "\tsubmit [s]           : submit analysis task" << endl
       << "\tquery [q]            : query job(s) status" << endl
       << "\toutput [o]           : find and retrieve task output files" << endl
       << "\tdelete [d]           : delete task or jobs from DB" << endl
       << "\tregister [r]         : register job wrapper script" << endl;
}







