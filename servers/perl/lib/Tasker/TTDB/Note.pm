use strict;

## @file
# This is the Note object

## @class
# The Note object
#
# This object holds a Note that the user entered.
#
package Tasker::TTDB::Note;

use Tasker::TTDB::DBI qw (get_dbh);

use Params::Validate qw (validate);

use Date::Calc::MySQL;

sub new
{
    my $class = shift;
    my %p = validate(@_, {
        note => 1,
        type => {
	    default => 1,
	},
        time => {
	    optional => 1,
	    isa => "Date::Calc",
	},
        user => {
	    optional => 1,
	    isa => "Tasker::TTDB::User",
	},
        project => {
	    optional => 1,
	    isa => "Tasker::TTDB::Project",
	},
	user_id => 0,
	project_id => 0,
    });

    bless { %p }, $class;
}

sub get
{
    my $class = shift;
    my $dbh = get_dbh('read');
    my %p = validate(@_, {
        id => 1,
    });
    my @ret;
    my @args = ();

    my $sql = "select * from notes";

    $sql .= ' where id = ?';

    my $sth = $dbh->prepare($sql);

    $sth->execute($p{id});

    map({bless $_, 'Tasker::TTDB::Note'} @{$sth->fetchall_arrayref({})});
}

sub create
{
    my $class = shift;

    my $dbh = get_dbh;

    my $self = $class->new(@_);

    my $st;

    my $st_id = $dbh->prepare(qq/select nextval('note_id_seq')/);

    my @extra = ();

    my $user_id = $self->user_id;
    my $project_id = $self->project_id;

    if (my $time = $self->time) {
	$st = $dbh->prepare("insert into notes (type, user_id, project_id, note, time) values (?, ?, ?, ?, ?)");
	push @extra, $time->mysql;
    } else {
	$st = $dbh->prepare("insert into notes (type, user_id, project_id, note, time) values (?, ?, ?, ?, now())");
    }

    eval {
	$st->execute($self->type, $user_id, $project_id, $self->note, @extra);
    }; if ($@) {
	$dbh->rollback;
	die $@;
    } else {
	$dbh->commit or die $dbh->errstr();
    }

    $self;
}

sub time
{
    my $self = shift;

    $self->{time};
}

sub type
{
    my $self = shift;

    $self->{type};
}

sub user_id
{
    my $self = shift;

    if (my $u = $self->{user}) {
        return $u->id;
    } else {
        return $self->{user_id};
    }
}

sub project_id
{
    my $self = shift;

    if (my $p = $self->{project}) {
        return $p->id;
    } else {
        return $self->{project_id};
    }
}

sub user
{
    my $self = shift;

    if ($self->user_id) {
	$self->{user} ||= Tasker::TTDB::User->get(id => $self->user_id);
    }
}

sub project
{
    my $self = shift;

    require Tasker::TTDB::Project;
    if ($self->project_id) {
	$self->{project} ||= Tasker::TTDB::Project->get(id => $self->project_id);
    }
    $self->{project};
}

sub date
{
    my $self = shift;

    Date::Calc::MySQL->new($self->{time});
}

sub text
{
    my $self = shift;

    $self->{note};
}

1;
__END__

=head1 NAME

Tasker::TTDB::Note - Perl interface to the tasker notes table

=head1 SYNOPSIS

  use Tasker::TTDB::Note;

  $auto = Tasker::TTDB::Note->new(user => $user);

  $auto = Tasker::TTDB::Note->create(
      project_id => I<projectid>,
      user_id => I<userid>,
  );

  $auto = Tasker::TTDB::Note->get(user => $user, role => 'bob'):

=head1 DESCRIPTION

Normally the I<note> is returned from the I<Tasker::TTDB::Notes> interface or somewhere else.

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

=item text

Get the text for the note.

=item time

Get the time that the note was created.
The time is returned in a Date::OO Object.

=item type

This returns the type of the note.

=item  user_id

return the I<user_id> for the object.

=item user

return a I<user> Object for the note.

=item  project_id

return the I<project_id> for the object.

=item project

return a I<project> Object for the note.

=item delete

This will delete the object from the database.

=back

=head1 SEE ALSO

L<Tasker::TTDB::Notes>

=head1 AUTHOR

"G. Allen Morris III" <gam3@gam3.net>

=cut
