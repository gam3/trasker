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

print "ok\n";
print "1..1\n";


