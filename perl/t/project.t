use strict;

use TTDB::Project;

use Test::More tests => 11;

our $user = $TTDB::DBI::user;
our $passwd = $TTDB::DBI::password;
our $database = $TTDB::DBI::database;

our $proj = TTDB::Project->get(id => 1);

pass();

is($proj->time, ' 0:00:00', 'one');
is($proj->alltime, ' 0:00:00', 'one all');

$proj = TTDB::Project->get(id => 5);
is($proj->time, ' 0:00:00', 'five');
is($proj->alltime, ' 0:00:00', 'five all');

use Data::Dumper;
print Dumper $TTDB::Project::get_time;

$proj = TTDB::Project->get(id => 8);
is($proj->time, ' 0:00:00', 'eight');
is($proj->alltime, ' 0:00:00', 'eight all');

is($proj->longname, 'Test::Bill', 'longname');

diag 'bob';

my $nproj = TTDB::Project->new(parent => $proj, name => 'bob');

$nproj->create;

pass('create');

diag 'bob';

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

