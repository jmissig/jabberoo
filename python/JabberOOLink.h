#include <boost/python.hpp>
#include <jabberoo.hh>

#include <sigc++/slot.h>
#include <sigc++/signal.h>
#include <sigc++/bind.h>
#include <sigc++/connection.h>

using namespace SigC;
using namespace judo;
using namespace jabberoo;
using namespace boost::python;

// Shorten the names
typedef boost::python::return_value_policy<boost::python::copy_const_reference> ccr;
typedef boost::python::return_internal_reference<> rir;

void exportSession();
void exportPresence();
void exportMessage();
void exportRoster();

void exportSigC();
