use strict;

use TTDB::Project;

use Test::More tests => 11;

our $user = $TTDB::DBI::user;
our $passwd = $TTDB::DBI::password;
our $database = $TTDB::DBI::database;

our $proj = TTDB::Project->get(id => 1);
is($proj->time, ' 0:00:00');
is($proj->alltime, ' 0:00:00');

$proj = TTDB::Project->get(id => 5);
is($proj->time, ' 0:00:00');
is($proj->alltime, ' 0:00:00');

$proj = TTDB::Project->get(id => 8);
is($proj->time, ' 0:00:00');
is($proj->alltime, ' 0:00:00');

is($proj->longname, 'g', 'longname');

pass();

my $nproj = TTDB::Project->new(parent => $proj, name => 'bob');

$nproj->create;

pass('create');

$proj = TTDB::Project->get(id => 8);

$proj->update(name => 'bill', parent_id => 1);

pass();
TTDB::Projects->flush();

$proj = TTDB::Project->get(id => 1);
eval {
    $proj->update(parent_id => 8);
};
diag($@);
pass($@);

