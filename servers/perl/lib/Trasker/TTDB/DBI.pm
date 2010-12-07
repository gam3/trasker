use strict;

## @file
# The database interface
#
#

## @class
# connect to the databse
#
# This object return a DBI handle;
# it is used to cache these handles and handle user name, password and host setup.
# It also can return readonly and read/write handles.
#
package Trasker::TTDB::DBI;

our $dbi;

use DBI;
require Exporter;

our @ISA = qw (Exporter);
our @EXPORT_OK = qw(get_dbh dbi_setup dbtype);  # symbols to export on request

our $database = '/tmp/test_trasker';
our $host = '';
our $user = '';
our $password = '';
our $db = 'sqlite';

use Params::Validate qw( validate );

sub dbtype
{
    'sqlite';
}

sub dbi_setup
{
    my %p = validate(@_, {
	database => 0,
	user => 0,
	password => 0,
	host => 0,
	type => 0,
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
    if (my $value = $p{type}) {
        $db = $value;
    }
}

sub get_dbh
{
    if (!$dbi) {
        if ($db eq 'sqlite3' or $db eq 'sqlite') {
	    $dbi = DBI->connect("dbi:SQLite:dbname=" . $database, $user, $password,
		{
		    RaiseError => 1,
		    PrintError => 0,
		    PrintWarn => 0,
		    AutoCommit => 0,
		}
	    );
	} else {
	    $dbi = DBI->connect("DBI:${db}:database=" . $database . ";host=$host", $user, $password,
		{
		    RaiseError => 1,
		    PrintError => 0,
		    PrintWarn => 0,
		    AutoCommit => 0,
		}
	    );
	}
    }
    return $dbi;
}

1;
__END__

=head1 NAME

Trasker::TTDB::DBI - Perl interface to the DBI

=head1 SYNOPSIS

  use Trasker::TTDB::DBI qw (get_dbi);

=head2 Constructor

=over

=back

=head2 Methods

=over

=item dbi_setup

=item get_dbh

=back

=head1 DESCRIPTION

=head1 AUTHOR

"G. Allen Morris III" <gam3@gam3.net>

=cut

