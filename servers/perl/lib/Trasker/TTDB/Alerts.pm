use strict;

## @file
# The Alerts package
#

## @class
# A list of Alert objects
#
# This is simply a list of Alert Objects
#
package Trasker::TTDB::Alerts;

use Trasker::TTDB::DBI qw (get_dbh);
use Trasker::TTDB::Alert;

use Params::Validate qw( validate validate_pos SCALAR BOOLEAN HASHREF OBJECT );

sub new
{
    my $class = shift;
    my @data = ();

    return bless { data => \@data }, $class;
}

sub get
{
    my $dbh = get_dbh;
    my $class = shift;

    my %p = validate(@_, {
        user_id => 0,
	updated => {
	    optional => 1,
	}
    });

    my $pid = $p{project_id};
    my $project = $p{project};
    if ($project) {
	$pid = $project->id;
    }

    my $sth;
    $sth = $dbh->prepare(<<SQL);
select *,
       'eof' as eof
  from alerts
 where user_id = ?
SQL
    $sth->execute($p{user_id});

    my @data;
    require Trasker::TTDB::UserProject;

    while (my $data = $sth->fetchrow_hashref()) {
	push(@data, bless { %$data }, 'Trasker::TTDB::Alert');
    }

    return bless { data => \@data }, $class;
}

sub entries
{
    my $self = shift;

    @{$self->{data}};
}

1;
__END__

