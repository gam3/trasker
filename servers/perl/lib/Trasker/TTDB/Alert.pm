use strict;
## @file
# The Alert object
#

## @class
# The Alert Object.
#
# A user is normally a human, but can be any object that can be in only
# one place at one time.
#
package Trasker::TTDB::Alert;

use Trasker::TTDB::DBI qw (get_dbh);

our $VERSION = '0.001';

use Params::Validate qw( validate validate_pos SCALAR BOOLEAN HASHREF OBJECT );

use Carp qw (croak);

sub new
{
    my $class = shift;
    my %p = validate(@_, {
        id => 0,
	start_time => 1,
	end_time => 0,
        title => 1,
        description => 1,
	alert_type_id => 0,
	recurs_id => 0,
    });

    return bless { %p }, $class;
}

sub create
{
    my $self = shift;

    die if $self->{id};

    my $dbh = get_dbh;

    my $sth_id;
    if (0) {
        $sth_id = $dbh->prepare('select LAST_INSERT_ID()');
    } else {
        $sth_id = $dbh->prepare("select currval('users_id_seq')");
    }
    my $sth = $dbh->prepare(<<SQL);
insert into users (name, fullname) values (?, ?)
SQL

    $sth->execute($self->{name}, $self->{fullname});
    $sth_id->execute();
    my $id = $sth_id->fetchrow_array();
    $self->{id} = $id;

    $dbh->commit();

    $self;
}

## @cmethod object get(%hash)
# create a user object.
# @param id is the user id [optional]
# @param user is the user name [optoinal]
# @return user object
sub get
{
    my $dbh = get_dbh;
    my $class = shift;

    my %p = validate(@_, {
        id => 0,
        user => 0,
    });

    croak(q(Can't have both a user and an id)) if (defined $p{user} && defined $p{id});
    croak(q(Must have a user or an id)) if (!defined $p{user} && !defined $p{id});

    my $sth;

    if (my $user = $p{user}) {
        $sth = $dbh->prepare(<<SQL);
select name,
       fullname,
       id,
       'eof' as eof
  from users
 where name = ?
SQL

        $sth->execute($user);
    } else {
        my $id = $p{id};
        $sth = $dbh->prepare(<<SQL);
select name,
       fullname,
       id,
       'eof'
  from users
 where id = ?
SQL

        $sth->execute($id);
    }

    my $data = $sth->fetchrow_hashref();

    die "No Alert" unless $data;

    return bless { %$data }, $class;
}

sub id
{
    my $self = shift;

    $self->{id} || die "No id";
}

sub get_timeslices_for_day
{
    my $self = shift;
    require Trasker::TTDB::TimeSlice;
    my %p = validate(@_, {
        date => {
            isa => 'Date::Calc',
        },
    });
    my $dbh = get_dbh;

    my $st = $dbh->prepare(<<SQL);
select * from timeslice
 where user_id = ?
   and start_time <= date(?) + interval '1 day'
   and end_time >= date(?)
SQL
    my @ret;

    $st->execute($self->id, $p{date}->mysql, $p{date}->mysql);

    while (my $row = $st->fetchrow_hashref()) {
        push @ret, Trasker::TTDB::TimeSlice->new(
           %$row,
           start_time => Trasker::Date->new($row->{start_time}),
           end_time => Trasker::Date->new($row->{end_time}),
        );
    }

    @ret;
}

1;
__END__

=head1 NAME

Trasker::TTDB::Alert - Perl interface to the tasker alert

=head1 SYNOPSIS

  use Trasker::TTDB::Alert;

  $alert = Trasker::TTDB::User->new(id => I<id>):

=head2 Constructor

=over

=item new

This creates a user object.  Use I<create> to make this object
preminate.

=item get

I<get> returns an alert object that describes the alert requested.

=back

=head2 Methods

=over

=item  id

=item  userid

=item  name

This is an alias to the userid function

=item fullname

=item projects

=item project

=item set_current_project

=item revert

=item auto_revert_project

=item auto_set_project

=item auto_get_project

=item current_project

=item has_project

=item add_task

=item add_note

=item create

This will create a user.  The name must be unique.

=item delete

This will delete a user, but only if the user has never been active.

=item day

Return information about a give day for the user.

=item days

Return information about a give range of days for the user.

=item get_timeslices_for_day

=item times

Get the timeslices for a given period

=back

=head1 DESCRIPTION

=head1 AUTHOR

"G. Allen Morris III" <gam3@gam3.net>

=cut
