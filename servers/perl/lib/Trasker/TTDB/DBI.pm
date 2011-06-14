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
our @EXPORT_OK = qw(get_dbh dbi_setup dbtype get_sti);  # symbols to export on request

our $database = '/tmp/test_trasker';
our $host = '';
our $user = '';
our $password = '';
our $db = 'sqlite';

use Params::Validate qw( validate );

sub dbtype
{
    return $db;
}

sub dbi_setup
{
    my %p = validate(@_, {
	database => 0,
	user => 0,
	password => 0,
	host => 0,
	port => 0,
	dbtype => 0,
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
    if (my $value = $p{dbtype}) {
        $db = $value;
    }
}

sub get_dbh
{
    if (!$dbi) {
        my $db_type = $db eq 'pgsql' ? 'Pg' : $db;
        if ($db_type eq 'sqlite3' or $db_type eq 'sqlite') {
	    $dbi = DBI->connect("dbi:SQLite:dbname=" . $database, $user, $password,
		{
		    RaiseError => 1,
		    PrintError => 0,
		    PrintWarn => 0,
		    AutoCommit => 0,
		}
	    );
	} else {
	    $dbi = DBI->connect("DBI:${db_type}:database=" . $database . ";host=$host", $user, $password,
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

our $sti;

sub get_sti
{
    my $name = shift;
    my $bob = $sti;
    if (!defined($bob)) {
	$bob = _set_stis();
    }

    return $bob->{$name};
}

sub _set_stis
{
    my $dbh = get_dbh();
    my $dbtype = dbtype();

    my $package = "require " . __PACKAGE__.'::'.$dbtype;

    eval $package;
    if ($@) {
warn $@;
    }
warn "bob $package";
    $sti = {
	user_create => $dbh->prepare(<<SQL),
insert into users (name, fullname) values (?, ?)
SQL
	timeslice_init => $dbh->prepare(<<SQL),
insert into timeslice
       (user_id, project_id, start_time)
values (      ?,          1, date_trunc('year', CURRENT_DATE))
SQL
	timeslice_get_current => $dbh->prepare(<<SQL),
select id, project_id, temporary, revert_to, auto_id
  from timeslice
 where end_id is NULL
   and user_id = ?
SQL
	timeslice_new => $dbh->prepare(<<SQL),
insert into timeslice
       (user_id, project_id, temporary, start_time, auto_id, revert_to, host)
values (      ?,          ?,         ?,      now(),       ?,         ?,    ?)
SQL
	timeslice_finish => $dbh->prepare(<<SQL),
update timeslice
   set elapsed = now() - start_time,
       end_time = now(),
       end_id = ?
 where id = ?
SQL
    };
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

