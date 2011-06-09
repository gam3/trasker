use strict;

$ENV{TEST_TRASKERD} = 1;
use Test::More;
#tests => 'no_plan';

use Trasker::TTDB::DBI;
use Trasker::TTDB::TimeSlice;
use Trasker::Date;

my $ts = Trasker::TTDB::TimeSlice->new(
    id => 2,
    'start_time' => '2001-01-01 01:00:00',
    'end_time' => '2001-01-01 02:00:00',
    'elapsed' => '01:00:00',
    'host' => 'host',
    'temporary' => 'temp',
    'user_id' => '3',
    'revert_to' => undef,
    'auto_id' => undef,
    'project_id' => 1,
    'end_id' => 1,
);

$Trasker::TTDB::DBI::dbi = DBI->connect('dbi:Mock:', '', '', { AutoCommit => 0 });
our $dbh = $Trasker::TTDB::DBI::dbi;

$dbh->{mock_add_resultset} = [[ 'rows' ], []];
$dbh->{mock_add_resultset} = [[ 'rows' ], []];
$dbh->{mock_add_resultset} = [[ 'id', 'user_id', 'project_id', 'start_time',            'end_time',              'temporary', 'auto_id', 'revert_to', 'host',  'elapsed', 'end_id'  ],
                              [  1,       3,          2, '2001-01-01 00:00:00', '2001-01-01 01:30:00', 'normal',  undef,   undef,    'host', '01:30:00', 2 ],
                              [  2,       3,          1, '2001-01-01 01:30:00', '2001-01-01 02:00:00', 'normal',  undef,   undef,    'host', '00:30:00', 3 ]];

our @ts = $ts->change_time('new_time'=> Trasker::Date->new('2001-01-01 00:30:00') );

is_deeply([@ts], [
           bless( {
                    'start_time' => '2001-01-01 00:00:00',
                    'elapsed' => '01:30:00',
                    'host' => 'host',
                    'temporary' => 'normal',
                    'end_time' => '2001-01-01 01:30:00',
                   'user_id' => 3,
                    'id' => 1,
                    'revert_to' => undef,
                    'auto_id' => undef,
                    'project_id' => 2,
		    'end_id' => 2,
                  }, 'Trasker::TTDB::TimeSlice' ),
           bless( {
                    'start_time' => '2001-01-01 01:30:00',
                    'elapsed' => '00:30:00',
                    'host' => 'host',
                   'temporary' => 'normal',
                    'end_time' => '2001-01-01 02:00:00',
                    'user_id' => 3,
                    'id' => 2,
                   'revert_to' => undef,
                    'auto_id' => undef,
                    'project_id' => 1,
		    'end_id' => 3,
                  }, 'Trasker::TTDB::TimeSlice' )
], 'check');

use Data::Dumper;
#diag Dumper \@ts;
#print Dumper $dbh->{mock_all_history};

done_testing();

