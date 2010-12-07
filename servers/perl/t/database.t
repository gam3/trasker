use strict;

use Test::More tests => 5;
use DBI;

use_ok('Trasker::TTDB::User');

$Trasker::TTDB::DBI::dbi = DBI->connect('dbi:Mock:', '', '', { AutoCommit => 0 });
our $dbh = $Trasker::TTDB::DBI::dbi;

$dbh->{mock_add_resultset} = [[ 'name', 'fullname', 'id', 'eof' ],
                              [ 'bob', 'Bob Smith', '1', 'eof' ]];

our $user;
$user = Trasker::TTDB::User->get(user => 'bob');

use Data::Dumper;
is($dbh->{mock_all_history}[0]{bound_params}[0], 'bob', 'by name');

$dbh->{mock_add_resultset} = [[ 'name', 'fullname', 'id', 'eof' ],
                              [ 'bob', 'Bob Smith', '1', 'eof' ]];

$user = Trasker::TTDB::User->get(id => 1);

is($dbh->{mock_all_history}[0]{bound_params}[0], 'bob', 'by id');

$user = Trasker::TTDB::User->create(
    name => 'bob',
    fullname => 'Bob Smith',
);

$user = Trasker::TTDB::User->create(
    name => 'bill',
    fullname => 'Bill Smith',
);

is($user->id, 2, "create id");

$dbh->{mock_clear_history} = 1;

eval {
    $user->current_project();
};
like($@, qr/^No current project/, 'no project');

$dbh->{mock_clear_history} = 1;

$dbh->{mock_add_resultset} = [[ 'project_id'], [ 1 ]];

my $project =
eval {
    $user->current_project();
};

#die Dumper $project, $dbh->{mock_all_history};

$dbh->{mock_clear_history} = 1;

$dbh->{mock_add_resultset} = [[ 'name', 'id', 'parent_id', 'user_id', 'description', 'eof' ],
                              [ 'bob', '1', undef, '1', 'Bob Smith', 'eof' ]];

$dbh->{mock_add_resultset} = [[ 'count(*)' ],
                              [ '1', ]];

my @projects = $user->projects();

print Dumper $dbh->{mock_all_history}, \@projects;


