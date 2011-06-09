use strict;

use Data::Dumper;
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

$dbh->{mock_add_resultset} = {
    sql => <<SQL,
select id, project_id, temporary, revert_to, auto_id
  from timeslice
 where end_id is NULL
   and user_id = ?
SQL
    results => [
	[ qw( id project_id   temporary  revert_to  auto_id ) ],
	[      1,         1,      undef,     undef,   undef   ],
    ]
};

$dbh->{mock_add_resultset} = {
    sql => <<SQL,
update timeslice
   set elapsed = now() - start_time,
       end_time = now(),
       end_id = ?
 where id = ?
SQL
    results => [
	[ qw( rows ) ],
	[        1   ],
    ]
};

Trasker::TTDB::DBI::_set_stis();

if (1) {
my $result =
eval {
    $user->set_current_project( host => "Host", project_id => 2 );
};
if ($@) {
    fail("set_current_project: ". $@);
} else {
    ok($result == 1, "set_current_project");
}
}

#diag Dumper $dbh->{mock_all_history};

done_testing();
