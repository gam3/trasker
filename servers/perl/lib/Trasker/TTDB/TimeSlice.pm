use strict;
## @file
## The Timeslice object
##
#

## @class
# A little slice of time
#
# A timeslice is a block of time attributed to a user and a project.
#
package Trasker::TTDB::TimeSlice;

use Params::Validate qw( validate validate_pos SCALAR BOOLEAN HASHREF OBJECT );

use Trasker::TTDB::DBI qw (get_dbh);

use Carp qw(croak);

=head1 NAME

Trasker::TTDB::TimeSlice - Perl interface to the tasker timeslice table

=head1 SYNOPSIS

  use Trasker::TTDB::TimeSlice;

  $user = Trasker::TTDB::User->new(user => 'bob', fullname => 'Robert Smith'):

  $user->create();

  $timeslice = Trasker::TTDB::TimeSlice->get(id => 1):

  my $new = $timeslice->split(time => $datetime);

=head1 DESCRIPTION

This package contains a single timeslice.

=head2 Constructors

=over

=item new()

This constructor is used by get to instaniciate this object.
Most users do not need to use this method.

=cut

sub new
{
    my $class = shift;
    my %p = validate(@_, {
        id => 0,
        start_time => 1,
        end_time => 1,
        temporary => 1,
        host => 1,
        elapsed => 1,
        user_id => 1,
        user_name => 0,
        auto_id => 1,
        revert_to => 1,
        project_id => 1,
    });

    my $self = bless({ %p }, $class);

    return $self;
}

=item get

This is the normal way to access a timeslice.

=cut

sub get
{
    my $class = shift;
    my $dbh = get_dbh();

    my %p = validate(@_, {
        id => 1,
	user_id => 0,
    });

    my @args = ($p{id});
    my $extra = '';

    if ($p{user_id}) {
         push(@args, $p{user_id});
	 $extra .= ' and user_id = ?';
    }

#FIX this needs security.  Only find for a certain set of users

    my $sth = $dbh->prepare(<<SQL);
select *
  from timeslice
 where id = ? $extra
SQL

    $sth->execute(@args);

    my $data = $sth->fetchrow_hashref();

    return $class->new(%$data);
}

sub id
{
    my $self = shift;

    $self->{id};
}

sub start_time
{
    my $self = shift;
    require Trasker::Date;

    Trasker::Date->new($self->{start_time});
}

sub end_time
{
    my $self = shift;

    $self->{end_time} ? Trasker::Date->new($self->{end_time}) : Trasker::Date->now();
}

sub duration
{
    my $self = shift;

    $self->end_time - $self->start_time;
}

sub user
{
    die;
    shift->{user};
}

sub project_id
{
    my $self = shift;

    $self->{project_id}
}

sub project
{
    my $self = shift;

    $self->{project} ||= Trasker::TTDB::Project->get(id => $self->{project_id});
}

sub elapsed
{
    my $self = shift;

    $self->end_time - $self->start_time;
}

sub count
{
    my $self = shift;

    1;
}

sub update
{
    my $self = shift;
    my $dbh = get_dbh('write');
    my %p = validate(@_, {
        id => 0,
	project_id => 0,
	old_project_id => 0,
    });
    my $id;
    my $old_value;
    my $new_value = $p{project_id} or die "Need new project id";
    my $field = "project_id";

    if (ref($self)) {
        $id = $self->id;
        $old_value = $self->project_id;
    } else {
        $id = $p{id} or croak "id is required";
        $old_value = $p{old_project_id} or croak "id is required";
    }

    my $stu = $dbh->prepare(<<'SQL');
update timeslice
   set project_id = ?
 where id = ? and project_id = ?
SQL
    $stu->execute($new_value, $id, $old_value);

    die "no update" unless $stu->rows;

    $dbh->commit;
}

=back

=head2 Accessors

=over

=cut

sub auto_id
{
    my $self = shift;
    $self->{auto_id};
}

sub from
{
    my $self = shift;
    $self->{from};
}

sub notes
{
    my $self = shift;
    require Trasker::TTDB::Notes;

    Trasker::TTDB::Notes->new(
        start_time => $self->start_time,
        end_time => $self->end_time,
    );
}


1;
__END__

=back

=head1 AUTHOR

"G. Allen Morris III" <gam3@gam3.net>

=cut
