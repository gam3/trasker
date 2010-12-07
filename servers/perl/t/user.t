use strict;

use Test::More tests => 9;

$Trasker::TTDB::DBI::dbi = DBI->connect('dbi:Mock:', '', '', { AutoCommit => 0 });

BEGIN {
    use_ok('Trasker::TTDB::User');
};

eval {
    Trasker::TTDB::User->new();
};
if ($@) {
    pass('no args');
} else {
    fail('no args');
}

our $dbh = $Trasker::TTDB::DBI::dbi;

$dbh->{mock_add_resultset} = [[ 'name', 'fullname', 'id', 'eof' ],
                              [ 'bob', 'Test Me', '1', 'eof' ]];

$dbh->{mock_add_resultset} = [[ 'name', 'fullname', 'id', 'eof' ],
                              [ 'bob', 'Test Me', '1', 'eof' ]];

our $user;

$user = Trasker::TTDB::User->get(id => 1);

is( $user->id, 1, 'test id' );
is( $user->name, "bob", 'test name' );
is( $user->fullname, "Test Me", 'test fullname' );

$user = Trasker::TTDB::User->get(user => 'bob');

is( $user->id, 1, 'test id' );
is( $user->name, "bob", 'test name' );
is( $user->fullname, "Test Me", 'test fullname' );

$dbh->{mock_clear_history} = 1;
$dbh->{mock_add_resultset} = [[ 'id', 'project_id', 'temporary', 'revert_to' ],
                              [ '99',            1,       undef,       undef ]];

$dbh->{mock_add_resultset} = {
    sql     => 'update timeslice
   set elapsed = julianday(\'now\') - julianday(start_time),
   end_time = datetime(\'now\')
 where id = ?
',
    results => [[ 'rows' ], []],
};

$dbh->{mock_clear_history} = 1;

my $bob =
eval {
    $user->set_current_project( host => "Host", project_id => 2 );
};
if ($@) {
    fail("set_current_project: ". $@);
} else {
    ok($bob == 1, "set_current_project");
}

use Data::Dumper;
print Dumper $dbh->{mock_all_history};

#done_testing();
