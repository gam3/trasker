use strict;

use Trasker::TTDB::Project;

use Test::More tests => 1;

our $user = $Trasker::TTDB::DBI::user;
our $passwd = $Trasker::TTDB::DBI::password;
our $database = $Trasker::TTDB::DBI::database;

our $proj = Trasker::TTDB::Project->get(id => 1);

pass();

