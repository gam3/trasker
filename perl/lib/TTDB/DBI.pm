use strict;

package TTDB::DBI;

our $dbi;

use DBI;
require Exporter;

our @ISA = qw (Exporter);
our @EXPORT_OK = qw(get_dbh dbi_setup);  # symbols to export on request

our $database = 'test_tasker';
our $host = '';
our $user = 'tasker';
our $password = 'seviceforward';
our $db = 'Pg';

use Params::Validate qw( validate );

sub dbi_setup
{
    my %p = validate(@_, {
	database => 0,
	user => 0,
	password => 0,
	host => 0,
    });
    if (my $value = $p{host}) {
        $host = $value;
    }
    if (my $value = $p{database}) {
        $database = $value;
    }
    if (my $value = $p{user}) {
        $user = $value;
    }
    if (my $value = $p{password}) {
        $password = $value;
    }
}

sub get_dbh
{
    if (!$dbi) {
	$dbi = DBI->connect("DBI:${db}:database=" . $database . ";", $user, $password,
	    {
		RaiseError => 0,
		PrintError => 1,
		PrintWarn => 1,
		AutoCommit => 0,
	    }
	);
    }
    return $dbi;
}

1;
__END__
