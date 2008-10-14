use strict;

use TTDB::Project;

use Test::More tests => 1;

our $user = $TTDB::DBI::user;
our $passwd = $TTDB::DBI::password;
our $database = $TTDB::DBI::database;

our $proj = TTDB::Project->get(id => 1);

pass();

