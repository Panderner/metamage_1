/*
	calc.cc
	-------
*/

#include "vc/calc.hh"

// more-libc
#include "more/string.h"

// plus
#include "plus/decimal.hh"
#include "plus/hexadecimal.hh"
#include "plus/integer_hex.hh"
#include "plus/string/concat.hh"

// vc
#include "vc/error.hh"
#include "vc/function_id.hh"


namespace vc
{
	
	using math::integer::cmp_t;
	
	
	using ::mempcpy;
	
	static inline
	char* mempcpy( char* p, const plus::string& s )
	{
		return (char*) mempcpy( p, s.data(), s.size() );
	}
	
	static plus::string hex( const plus::string& s )
	{
		return plus::hex_lower( s.data(), s.size() );
	}
	
	static
	const plus::integer& nonzero( const plus::integer& x )
	{
		if ( x.is_zero() )
		{
			DOMAIN_ERROR( "division by zero" );
		}
		
		return x;
	}
	
	static
	bool equal( const Value& a, const Value& b )
	{
		if ( a.type != b.type )
		{
			if ( a.type == Value_empty_list  ||  b.type == Value_empty_list )
			{
				return false;
			}
			
			TYPE_ERROR( "mismatched types in equality relation" );
		}
		
		switch ( a.type )
		{
			case Value_empty_list:
				return true;
			
			case Value_boolean:
			case Value_number:
				return a.number == b.number;
			
			case Value_string:
				return a.string == b.string;
			
			case Value_function:
				return a.function == b.function;
				break;
			
			case Value_pair:
				SYNTAX_ERROR( "equality for lists unimplemented" );
				break;
			
			default:
				INTERNAL_ERROR( "unsupported type in equal()" );
				break;
		}
		
		return false;
	}
	
	static
	cmp_t compare( const Value& a, const Value& b )
	{
		if ( a.type != b.type )
		{
			TYPE_ERROR( "mismatched types in compare()" );
		}
		
		switch ( a.type )
		{
			case Value_number:
				return compare( a.number, b.number );
			
			case Value_string:
				return compare( a.string, b.string );
			
			default:
				TYPE_ERROR( "unsupported type in compare()" );
				break;
		}
		
		return 0;
	}
	
	static
	plus::string::size_type composite_length( const Value& value )
	{
		switch ( value.type )
		{
			case Value_empty_list:  // ""
			case Value_string:
			case Value_function:
				return value.string.size();
			
			case Value_boolean:
				return 4 + value.number.is_zero();  // "true" or "false"
			
			case Value_number:
				return decimal_length( value.number );
			
			case Value_pair:
				break;
			
			default:
				INTERNAL_ERROR( "unsupported type in composite_length()" );
				break;
		}
		
		Expr* expr = value.expr.get();
		
		plus::string::size_type total = composite_length( expr->left );
		
		while ( Expr* next = expr->right.expr.get() )
		{
			total += composite_length( next->left );
			
			expr = next;
		}
		
		return total + composite_length( expr->right );
	}
	
	static
	char* make_string( char* p, const Value& value )
	{
		switch ( value.type )
		{
			case Value_empty_list:  // ""
			case Value_string:
			case Value_function:
				return mempcpy( p, value.string );
			
			case Value_boolean:
				if ( value.number.is_zero() )
				{
					return (char*) mempcpy( p, STR_LEN( "false" ) );
				}
				else
				{
					return (char*) mempcpy( p, STR_LEN( "true" ) );
				}
			
			case Value_number:
				return encode_decimal( p, value.number );
			
			case Value_pair:
				break;
			
			default:
				INTERNAL_ERROR( "unsupported type in make_string()" );
				break;
		}
		
		Expr* expr = value.expr.get();
		
		p = make_string( p, expr->left );
		
		while ( Expr* next = expr->right.expr.get() )
		{
			p = make_string( p, next->left );
			
			expr = next;
		}
		
		return make_string( p, expr->right );
	}
	
	static
	Value v_str( const Value& value )
	{
		const plus::string::size_type size = composite_length( value );
		
		plus::string result;
		
		char* p = result.reset( size );
		
		make_string( p, value );
		
		return result;
	}
	
	static
	Value v_bool( const Value& arg )
	{
		switch ( arg.type )
		{
			default:
				INTERNAL_ERROR( "invalid type in v_bool()" );
			
			case Value_empty_list:
				return false;
			
			case Value_boolean:
				return arg;
			
			case Value_number:
				return ! arg.number.is_zero();
			
			case Value_string:
				return ! arg.string.empty();
			
			case Value_function:
			case Value_pair:
				return true;
		}
	}
	
	static
	Value v_hex( const Value& arg )
	{
		switch ( arg.type )
		{
			default:  TYPE_ERROR( "invalid argument to hex()" );
			
			case Value_number:  return hex( arg.number );
			case Value_string:  return hex( arg.string );
		}
	}
	
	static
	Value v_abs( const Value& arg )
	{
		if ( arg.type != Value_number )
		{
			TYPE_ERROR( "invalid argument to abs()" );
		}
		
		return abs( arg.number );
	}
	
	static
	Value v_half( const Value& arg )
	{
		if ( arg.type != Value_number )
		{
			TYPE_ERROR( "invalid argument to half()" );
		}
		
		return half( arg.number );
	}
	
	static
	Value calc_function( unsigned f, const Value& arg )
	{
		switch ( f )
		{
			default:  INTERNAL_ERROR( "unimplemented function" );
			
			case Function_abs:   return v_abs ( arg );
			case Function_bool:  return v_bool( arg );
			case Function_half:  return v_half( arg );
			case Function_hex:   return v_hex ( arg );
			case Function_str:   return v_str ( arg );
		}
	}
	
	static
	size_t count( const Value& list )
	{
		if ( list.type != Value_pair )
		{
			return list.type != Value_empty_list;
		}
		
		Expr* expr = list.expr.get();
		
		size_t total = count( expr->left );
		
		while ( Expr* next = expr->right.expr.get() )
		{
			total += count( next->left );
			
			expr = next;
		}
		
		return total + count( expr->right );
	}
	
	static
	Value calc_unary( op_type op, const Value& v )
	{
		if ( op == Op_const  ||  op == Op_var )
		{
			SYNTAX_ERROR( "const/var operand not a symbol" );
		}
		
		if ( op == Op_unary_count )
		{
			return count( v );
		}
		
		switch ( v.type )
		{
			case Value_empty_list:
			case Value_boolean:
			case Value_number:
				switch ( op )
				{
					case Op_unary_plus:   return  v.number;
					case Op_unary_minus:  return -v.number;
					
					default:  break;
				}
				
				break;
			
			case Value_string:
				SYNTAX_ERROR( "unary operator not defined for string values" );
				break;
			
			case Value_pair:
				SYNTAX_ERROR( "unary operator not defined for lists" );
				break;
			
			default:
				break;
		}
		
		INTERNAL_ERROR( "unsupported operator in calc_unary()" );
		
		return Value();
	}
	
	static
	Value calc( const plus::integer&  left,
	            op_type               op,
	            const plus::integer&  right )
	{
		switch ( op )
		{
			case Op_add:       return left + right;
			case Op_subtract:  return left - right;
			case Op_multiply:  return left * right;
			case Op_divide:    return left / nonzero( right );
			case Op_remain:    return left % nonzero( right );
			case Op_modulo:    return modulo( left, nonzero( right ) );
			
			case Op_empower:   return raise_to_power( left, right );
			
			default:
				break;
		}
		
		INTERNAL_ERROR( "unsupported operator in calc()" );
		
		return Value();
	}
	
	static
	Value make_pair( const Value& left, const Value& right )
	{
		if ( left.type != Value_pair )
		{
			return Value( left, right );
		}
		
		Expr& expr = *left.expr.get();
		
		return Value( expr.left, make_pair( expr.right, right ) );
	}
	
	static
	plus::string repeat( const plus::string& s, plus::integer n )
	{
		typedef plus::string::size_type size_t;
		
		if ( n.is_negative() )
		{
			DOMAIN_ERROR( "negative string multiplier" );
		}
		
		const char*  data = s.data();
		const size_t size = s.size();
		
		size_t n_times = n.clipped();
		
		n *= size;
		
		if ( n > size_t( -1 ) )
		{
			DOMAIN_ERROR( "excessively large string multiplier" );
		}
		
		const size_t n_bytes = n.clipped();
		
		plus::string result;
		
		char* p = result.reset( n_bytes );
		
		while ( n_times-- > 0 )
		{
			p = (char*) mempcpy( p, data, size );
		}
		
		return result;
	}
	
	static
	Value repeat_list( const Value& list, const Value& factor )
	{
		if ( factor.type != Value_number )
		{
			TYPE_ERROR( "non-numeric list repetition factor" );
		}
		
		if ( factor.number.is_negative() )
		{
			DOMAIN_ERROR( "negative list repetition factor" );
		}
		
		if ( factor.number.is_zero()  ||  list.type == Value_empty_list )
		{
			return Value_empty_list;
		}
		
		if ( factor.number > size_t( -1 ) )
		{
			DOMAIN_ERROR( "excessively large list multiplier" );
		}
		
		size_t n = factor.number.clipped();
		
		if ( n == 1 )
		{
			return list;
		}
		
		Value result = list;
		
		while ( --n > 0 )
		{
			result = make_pair( list, result );
		}
		
		return result;
	}
	
	Value calc( const Value&  left,
	            op_type       op,
	            const Value&  right )
	{
		if ( left.type == Value_dummy_operand )
		{
			return calc_unary( op, right );
		}
		
		switch ( op )
		{
			case Op_function:
			case Op_named_unary:
				if ( left.type == Value_function )
				{
					return left.function( right );
				}
				
				if ( left.type != Value_builtin_function )
				{
					SYNTAX_ERROR( "attempted call of non-function" );
				}
				
				return calc_function( left.number.clipped(), right );
			
			case Op_equal:    return equal( left, right );
			case Op_unequal:  return ! equal( left, right );
			
			case Op_lt:   return compare( left, right ) <  0;
			case Op_lte:  return compare( left, right ) <= 0;
			case Op_gt:   return compare( left, right ) >  0;
			case Op_gte:  return compare( left, right ) >= 0;
			
			default:  break;
		}
		
		if ( op == Op_list )
		{
			if ( left.type == Value_empty_list )
			{
				return right;
			}
			
			if ( right.type == Value_empty_list )
			{
				return left;
			}
			
			return make_pair( left, right );
		}
		
		if ( op == Op_repeat )
		{
			return repeat_list( left, right );
		}
		
		if ( left.type == right.type )
		{
			switch ( left.type )
			{
				case Value_empty_list:
					SYNTAX_ERROR( "operator not defined for empty list" );
				
				case Value_boolean:
					SYNTAX_ERROR( "operator not defined for boolean values" );
				
				case Value_number:
					return calc( left.number, op, right.number );
				
				case Value_string:
					SYNTAX_ERROR( "operator not defined for string values" );
				
				case Value_pair:
					SYNTAX_ERROR( "operator not defined for lists" );
				
				default:
					break;
			}
		}
		
		if ( op == Op_multiply  &&  left.type == Value_string )
		{
			if ( right.type == Value_boolean  ||  right.type == Value_number )
			{
				return repeat( left.string, right.number );
			}
		}
		
		SYNTAX_ERROR( "operator not defined on mixed types" );
		
		return Value();
	}
	
}
