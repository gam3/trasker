use strict;
use warnings;

package Tasker;

use CGI;
use Carp qw (croak);
use URI::Escape qw(uri_escape);

use Params::Validate qw( validate validate_pos SCALAR BOOLEAN HASHREF OBJECT );

use TTDB::DBI qw (dbi_setup);

dbi_setup(
    database => 'tasker',
    user => 'gam3',
    password => 'awnmwad',
);

sub init
{
    my $self = shift;

    undef;
}

sub header
{
    print "Content-Type: text/html; charset=iso-8859-1\n";
}

sub title
{
   "Tasker Unknown Page";
}

sub head
{
    my $self = shift;
    my $title = $self->title;
    my $type = $self->{type};
<<'EOP' .
<html>
 <head>
EOP
  "<title>$title</title>\n" .
<<'EOP';
  <link href="/ahe/css/turbo.css" rel="stylesheet" type="text/css"/>
  <script type="text/javascript" src="/ahe/javascript/turbo.js"></script>
 </head>
EOP
}

sub response_id
{
    my $self = shift;
    my $rid = $self->param('ResponseID');

    $rid;
}

sub link
{
    my $self = shift;
    die caller if (@_ % 2);
    my $default = { @_ };
    return sub {
        my $args = { %{$default}, @_ };
        my $uri = URI->new();
        $uri->path($args->{_path});
        delete $args->{_path};
        my $query;
        for my $key (keys %$args) {
            if ($args->{$key}) {
                if ($query) {
                    $query .= '&amp;';
                }
                if (ref $args->{$key} eq 'ARRAY') {
use Data::Dumper;
for my $x (@{$args->{$key}}) {
    die caller, Dumper $args unless $x;
}
                    $query .= "$key=" . join(',', @{$args->{$key}});
                } else {
                    $query .= "$key=" . $args->{$key};
                }
            }
        }

        $uri->query($query);

        $uri->as_string;
    }
}

sub start_body
{
    '<body>';
}

sub end_body
{
    '</body>';
}

sub user
{
    undef;
}

sub template_type
{
    my $self = shift;

    $self->{template_type} || 'europa_blue';
}

sub new
{
    my $class = shift;
    my %p = validate(@_, {
	extra => 0,
	referer => 0,
	self_url => 1,
    });

    bless { %p }, $class;
}

sub parameters
{
    {};
}

sub is_debug_flag
{
    0;
}

sub set_params
{
    my $self = shift;
    my %p = @_;
    for my $key (keys %p) {
	if ($key eq 'SubscriberID') {
	    for my $x (split("\0", $p{$key})) {
		push(@{$self->{param}->{$key}}, split(",", $x));
	    }
	} else {
	    $self->{param}->{$key} = [ split("\0", $p{$key}) ];
	}
    }

    my $params = $self->parameters();

    my $x;

    for my $key (keys %$params) {

    }
    undef;
}

sub form_params
{
    my $self = shift;
    (
        %{$self->{param} || {}},
    );
}

sub params
{
    my $self = shift;
    $self->{param};
}

sub param
{
    my $self = shift;
    my $name = shift;

    return undef unless exists $self->{param}{$name};

    if (wantarray) {
        @{$self->{param}{$name}};
    } else {
        croak("too many args to $name") if (@{$self->{param}{$name}} > 1);
        $self->{param}{$name}->[0];
    }
}

sub param_clean
{
    my $self = shift;
    my $name = shift;

    return undef unless exists $self->{param}{$name};
    delete $self->{param}{$name};
}

sub html
{
    "This is an empty page.";
}

sub self_url
{
    shift->{self_url};
}

sub footer
{
  my $self_url = CGI::self_url();
  $self_url =~ s/%3A/:/g;
  $self_url =~ s/;/\&/g;
  $self_url = uri_escape($self_url);

  my $ret;
  $ret = <<EOP;
  <div class="all">
  <div style="text-align: right">
   <p>
    <img
      src="http://www.w3.org/Icons/valid-xhtml10" alt="Valid XHTML 1.0 Transitional" height="31" width="88" />
    <img style="border:0;width:88px;height:31px"
     src="/images/vcss.png"
     alt="Valid CSS!" />
   </p>
  </div>
  </div>
EOP
}

our $template_path = '../templates/';

sub get_template
{
    my $self = shift;
    require  WWW::Template;

    $WWW::Template::path = $template_path;

    my $template = WWW::Template->new(@_);

    return $template;
}

sub get_form
{
    my $self = shift;
    require  WWW::Template::Form;
    my $form = WWW::Template::Form->new(@_);

    return $form;
}

sub add_warning
{
    my $self = shift;

    $self->{warning} .= shift(@_) . '<br/>';
    
}

sub redirect
{
    my $self = shift;

    require TurboSchedule::Exception;

    $self->{redirected}++;

    my $link = $self->link(
	SubscriberID => $self->param('SubscriberID'),
	Token => $self->param('Token'),
	@_);

    TurboSchedule::Exception::Redirect(
        path => $link->(),
    );
}

sub now
{
    my $self = shift;

    Tasker::Date->now();
}

sub set
{
    my $self = shift;
    my $name = shift;
    my $value = shift;
    die if @_;

    $self->{_data}{$name} = $value;
}
sub get
{
    my $self = shift;
    my $name = shift;

    $self->{_data}{$name};
}

1;
__END__

=head1 NAME

TurboSchedule::Pg - This is the base object of all the Webpage objects.

=head1 SYNOPSIS

This objects contains the default methods that each webpage object needs.

=head1 DESCRIPTION

=over

=item new

The constructor.

=item init

Initialize the object this may be used in place of new.
and is called right after set_params.

=item set_params

This method uses CGI or Apache to set the params for Pg.

=back

=over

=item header

This return the default HTTP header.

=item head

This returns the `head' section of a page.

=item parameters

By default no parameters are required.

=item html

This method returns the main portion of html from each web page. Each
TurboSchedule::Pg module needs to have one of these methods.

=item param

This is similar to the CGI::param() method. It works in the same
manner but can do some extra checking with the information that
parameter() returns.

=item foot

The text returned from this is output right before the body end.

=back

=head1 AUTHOR

"G. Allen Morris III" <gam3@gam3.net>

=head1 COPYRIGHT

This text is a trade secret of Almond Hill Enterprises

=cut
