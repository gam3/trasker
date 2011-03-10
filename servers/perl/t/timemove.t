use strict;
use IO::Scalar;

$ENV{TEST_TRASKERD} = 1;
use Test::More;
#tests => 'no_plan';

push @ARGV, "x";
push @ARGV, "-c", "t/config_file";
require("script/traskerd");

delete $SIG{'__DIE__'};
delete $SIG{'__WARN__'};

our $data = "";
our $client = IO::Scalar->new(\$data);

$main::hash->{'_clients'}{overload::StrVal $client}{authorized} = bless { ok => 1 }, "Trasker::TTDB::Authorize";
$main::hash->{'_clients'}{overload::StrVal $client}{client} = $client;

$Trasker::TTDB::DBI::dbi = DBI->connect('dbi:Mock:', '', '', { AutoCommit => 0 });
our $dbh = $Trasker::TTDB::DBI::dbi;

use Data::Dumper;

$dbh->{mock_add_resultset} = [
    [ 'name', 'fullname', 'id' ],
    [ 'gam3', 'G. Allen Morris III', 1 ] 
];
$dbh->{mock_add_resultset} = [
    [ qw(id user_id project_id start_time end_time temporary auto_id revert_to host elapsed) ],
    [ 2, 1, '2011-01-15 08:38:26.044608', '2011-01-15 08:42:43.66477', 'normal', undef, undef, 'harpo', '00:04:17.620162' ],
];

$dbh->{mock_add_resultset} = [[ 'rows' ], []];
$dbh->{mock_add_resultset} = [[ 'rows' ], []];
$dbh->{mock_add_resultset} = [[ 'id', 'user_id', 'project_id', 'start_time',            'end_time',  'temporary', 'auto_id', 'revert_to', 'host',  'elapsed'  ],
                              [  1,    3,          2,  '2001-01-01 00:00:00', '2001-01-01 01:30:00', 'normal',    undef,      undef,      'host',  '01:30:00' ],
                              [  2,    3,          1,  '2001-01-01 01:30:00', '2001-01-01 02:00:00', 'normal',    undef,      undef,      'host',  '00:30:00' ]];

$dbh->{mock_add_resultset} = 
{
sql => <<SQL,
select project.name as name,
       project.id as id,
       project.parent_id as parent_id,
       project.user_id as user_id,
       project.description as description,
       'eof'
  from project
SQL
results => [
    [ qw( id parent_id   name  user_id  hide  description client_id ) ],
    [      1,    undef, 'one',  undef,  undef, 'bye',          undef  ],
    [      2,    undef, 'two',  undef,  undef, 'hello',        undef  ],
]
};

eval {
    diag timemove(
	$client,    # my $client = shift;
	"gam3 bob", # my $data = shift;
	"gam3",     # my $user_id = shift;
	2,          # my $timeslice_id = shift;
	'2011-01-15 07:42:43',
    );
}; if ($@) {
    my $error = $@;
    diag $error;
}
#diag Dumper $dbh->{mock_all_history};
open my $out, "> bob";
print $out $data;
is($data, <<EOP, 'output');
timeslice	gam3	1	2	0	""	2001-01-01 00:00:00	+0+0+0+1+30+0
entry	gam3	two	2	0	 0:00:00	 0:00:00	1
timeslice	gam3	2	1	0	""	2001-01-01 01:30:00	+0+0+0+0+30+0
entry	gam3	one	1	0	 0:00:00	 0:00:00	1
EOP

done_testing();
