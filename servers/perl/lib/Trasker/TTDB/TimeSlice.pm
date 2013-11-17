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

use Carp qw (croak);

$Carp::Internal{__PACKAGE__}++;

use Params::Validate qw( validate validate_pos SCALAR BOOLEAN HASHREF OBJECT );

use Trasker::TTDB::DBI qw (get_dbh);
use Trasker::Date;

use Carp qw(croak carp);

=head1 NAME

Trasker::TTDB::TimeSlice - Perl interface to the tasker timeslice table

=head1 SYNOPSIS

  use Trasker::TTDB::TimeSlice;

  $user = Trasker::TTDB::TimeSlice->get(id => 1, user_id => 1):

=head1 DESCRIPTION

This package contains a single timeslice.

=head2 Constructors

=over

=item new

This constructor is used by C<get> to instantiate this object.
Most users do not need to use this method.

=cut

sub new
{
    my $class = shift;
    my %p = validate(@_, {
        id => 0,
        start_time => 1,
        end_time => 0,
        end_id   => 1,
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

This method takes 2 arguments the ID of the timeslice and and the user that the timeslice is for.

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
         $extra .= ' and ts.user_id = ?';
    }

    my $sth = $dbh->prepare(<<SQL);
select ts.id,
       ts.start_time,
       te.start_time as end_time,
       ts.end_id,
       ts.temporary,
       ts.host,
       coalesce(te.start_time, now()) - ts.start_time as elapsed,
       ts.user_id,
       ts.auto_id,
       ts.revert_to,
       ts.project_id
  from timeslice ts
   left join timeslice te on ts.end_id = te.id
 where ts.id = ? $extra
SQL

    $sth->execute(@args);

    my $data = $sth->fetchrow_hashref();
use Data::Dumper;
print Dumper $data;

    return $class->new(%$data);
}

=back

=head2 Methods

=over

=item update

The I<update> method changes the project_id for a given timeslise

  $updated_timeslice =
  $ts->update(
      project_id => I<n>,
  );

=cut

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

    return $self;
}

=item split

The I<split> devides a timeslice into 2 pieces.
pieces.

  $new_timeslice =
  $ts->split(
      time => [Trasker::Date],
  );

=cut

sub split
{
    my $self = shift;
    my $dbh = get_dbh('write');
    my %p = validate(@_, {
        time => {
	    isa => 'Trasker::Date',
	},
    });

    $self->reload();

    return Trasker::TTDB::TimeSlice->new;
}

=item change_time

The I<change_time> method changes the starttime for a timeslice

  $updated_timeslice =
  $ts->change_time(
      new_time => Trasker::Date->new();
  );

  The new time must be in the range of the previous time slice and the current timeslice.

=cut

sub change_time
{
    my $self = shift;
    my $dbh = get_dbh('write');
    my @ot;
    my @project_ids = ();
    my $class = ref($self);

    my %p = validate(@_, {
        new_time => {
	    isa => 'Trasker::Date',
	},
    });
    my $id;
    my $old_value;
    my $old_time = Trasker::Date->new($self->{start_time});
    my $new_time = $p{new_time};
    my $user_id = $self->user_id;

    my $stc = $dbh->prepare(<<'SQL');
update timeslice
   set start_time = ?
 where start_time = ? and user_id = ?
SQL
    eval {
	$stc->execute($new_time->mysql, $old_time->mysql, $user_id);
    }; if ($@) {
	$dbh->rollback;
	die "could not update";
    } else {
	$dbh->commit;
        my $ts = Trasker::TTDB::TimeSlice->get(id => $self->id);
	push @ot, $ts;
        my $sti = $dbh->prepare(<<'SQL');
select id from timeslice where (end_id = ?)
SQL
	$sti->execute($self->id);
        if ($sti->rows == 1) {
          my $row = $sti->fetchrow_hashref();
          my $ts = Trasker::TTDB::TimeSlice->get(id => $row->{'id'});
	  push @ot, $ts;
	}
    }
    return @ot;
}

=back

=head2 Accessors

=over

=item id

The timeslice I<id>.  This is a unique integer for each timeslice.

=cut

sub id
{
    my $self = shift;

    $self->{id};
}

=item current

The timeslice is the current timeslice.  It is the active timeslice.

=cut

sub current
{
    my $self = shift;

    !defined($self->{end_time});
}

=item start_time

The timeslice I<start_time>. A Trasker::Date object that represents the start time of the
the timeslice.

=cut

sub start_time
{
    my $self = shift;
    require Trasker::Date;

    Trasker::Date->new($self->{start_time});
}

=item end_time

The timeslice I<end_time>. A Trasker::Date object that represents the end time of the
the timeslice.

=cut

sub end_time
{
    my $self = shift;

    $self->{end_time} ? Trasker::Date->new($self->{end_time}) : Trasker::Date->now();
}

=item duration

The duration of the timeslice.  This is exactly equal to end_time - start_time.  And
is a Trasker::Date difference object;

=cut

sub duration
{
    my $self = shift;

    $self->end_time - $self->start_time;
}


=item user_id

The I<user_id> of the user whos time the timeslice is representing.

=cut

sub user_id
{
    shift->{user_id};
}

=item project_id

The I<user_id> of the user whos time the timeslice is representing.

=cut

sub project_id
{
    my $self = shift;

    $self->{project_id}
}

=item count

Always 1.

=cut

sub count
{
    my $self = shift;

    1;
}

=item count

The auto_id that if the timeslice was created by an auto_select deamon.

=cut

sub auto_id
{
    my $self = shift;
    $self->{auto_id} // '0';
}

=item from

The device that created the timeslice.

=cut

sub from
{
    my $self = shift;

    $self->{from} // '';
}

=item notes

A list of notes that were created while this timeslice was active.

=cut

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
