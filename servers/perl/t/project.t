use strict;

use Tasker::TTDB::Project;

use Test::More tests => 1;

our $user = $Tasker::TTDB::DBI::user;
our $passwd = $Tasker::TTDB::DBI::password;
our $database = $Tasker::TTDB::DBI::database;

our $proj = Tasker::TTDB::Project->get(id => 1);

pass();

