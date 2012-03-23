package Compile::Driver::Options;

use warnings;
use strict;

my %Spec_for_option = qw
(
	R opt
);

my @Specs;

sub specs
{
	return @Specs;
}

sub set_option
{
	my ( $option ) = @_;
	
	my $spec = $Spec_for_option{ $option } or die "No such option '$option'\n";
	
	push @Specs, $spec;
}

sub set_options
{
	my ( @argv ) = @_;
	
	my @args;
	
	while ( my $arg = shift )
	{
		if ( $arg !~ m{^ - }x )
		{
			push @args, $arg;
		}
		elsif ( $arg !~ m{^ -- }x )
		{
			my @chars = split "", $arg;
			
			shift @chars;  # lose the '-'
			
			set_option( $_ ) foreach @chars;
		}
		else
		{
			die "No long options yet\n";
		}
	}
	
	return @args;
}

1;

