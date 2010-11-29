use strict;

use Test::More tests => 4;
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

#print Dumper $dbh->{mock_all_history};
is($user->id, 2, "create id");

eval {
our $up = $user->current_project();
};

print Dumper $dbh->{mock_all_history};

