
## @file
## The Notes object
##
#

## @class
# A list of notes
#
# This Object simply holds a list of Note objects
# 
package Trasker::TTDB::Notes;
use strict;

use Params::Validate qw (validate ARRAYREF);

use Trasker::TTDB::DBI qw (get_dbh dbtype);

sub new
{
    my $class = shift;

    my %p = validate(@_, {
        date => {
	    optional => 1,
	    isa => 'Trasker::Date',
	},
	today => 0,
        start_time => {
	    optional => 1,
	    isa => 'Trasker::Date',
	},
        end_time => {
	    optional => 1,
	    isa => 'Trasker::Date',
	},
	project_ids => {
	    optional => 1,
	    type => ARRAYREF,
	},
	user_ids => {
	    optional => 1,
	    type => ARRAYREF,
	},
    });

    bless { %p }, $class;
}

sub entries
{
    my $self = shift;
    my $dbi = get_dbh;
    my @ret;
    my @args = ();

    my $sql = "select * from notes where 1 = 1";
    my @projects;
    my @users;

    if (my $a = $self->{project_ids}) {
        $sql .= sprintf(' and project_id in (%s)', join(', ', map({'?'} @{$a})));
	push(@args, @$a);
    }

    if (my $a = $self->{user_ids}) {
        $sql .= sprintf(' and user_id in (%s)', join(', ', map({'?'} @{$a})));
	push(@args, @$a);
    }

    my $now = 'now()';
    if (dbtype eq 'sqlite') {
        $now = q[date('now')];
    }

    if ($self->{today}) {
        $sql .= ' and date(time) = date($now)';
    }
    if ($self->{date}) {
        $sql .= ' and date(time) = date(?)';
	push(@args, $self->{date}->mysql);
    }
    if ($self->{start_time}) {
        $sql .= ' and time between ? and ?';
	push(@args, $self->{start_time}->mysql, $self->{end_time}->mysql);
    }

    $sql .= ' order by time';
    $sql .= ' limit 100';

    my $sth = $dbi->prepare($sql);

    $sth->execute(@args);
    require Trasker::TTDB::Note;

    @ret = map({bless $_, 'Trasker::TTDB::Note'} @{$sth->fetchall_arrayref({})});

    @ret;
}

1;
__END__

=head1 NAME

Trasker::TTDB::Notes - Perl interface to the tasker notes table

=head1 SYNOPSIS

  use Trasker::TTDB::Notes;

  $auto = Trasker::TTDB::Notes->entries(user => $user, project_ids => [ 11 ]):

=head1 DESCRIPTION

=head2 Constructor

=over

=item new

This does not put the object into the database.

=item get

Get an object from the database.

=item create

This will create a auto entry, if one does not exist. 

=back

=head2 Methods

=over

=item  id

return the I<id> of the object.

=item  user_id

return the I<user_id> for the object.

=item  project_id

return the I<project_id> for the object.

=item delete

=item entries

This will delete the object from the database.

=back

=head1 SEE ALSO

L<Trasker::TTDB::Note>

=head1 AUTHOR

"G. Allen Morris III" <gam3@gam3.net>

=cut

