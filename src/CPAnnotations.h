/*
 *  CPAnnotations.h
 *  CorePlatform
 *
 *  Created by Ben Vanik on 1/15/11.
 *  Copyright 2011 Ben Vanik. All rights reserved.
 *
 */

#pragma once

#include <CPPlatform.h>

#if CP_LIKE(WIN32)

// http://msdn.microsoft.com/en-us/library/aa383701.aspx
#include <specstrings.h>

#define sal_in                                  __notnull   __in
#define sal_in_opt                              __maybenull __in_opt
#define sal_in_z                                __notnull   __in_z
#define sal_in_z_opt                            __maybenull __in_z_opt
#define sal_out                                 __notnull   __out
#define sal_out_opt                             __maybenull __out_opt
#define sal_out_z                               __notnull   __out_z
#define sal_out_z_opt                           __notnull   __out_z_opt
#define sal_inout                               __notnull   __inout
#define sal_inout_opt                           __maybenull __inout_opt
#define sal_inout_z                             __notnull   __inout_z
#define sal_inout_z_opt                         __maybenull __inout_z_opt

#define sal_in_bcount(size)                     __notnull   __in_bcount(size)
#define sal_in_bcount_opt(size)                 __maybenull __in_bcount_opt(size)
#define sal_out_bcount(size)                    __notnull   __out_bcount(size)
#define sal_out_bcount_opt(size)                __maybenull __out_bcount_opt(size)
#define sal_inout_bcount(size)                  __notnull   __inout_bcount(size)
#define sal_inout_bcount_opt(size)              __maybenull __inout_bcount_opt(size)
#define sal_in_ecount(size)                     __notnull   __in_ecount(size)
#define sal_in_ecount_opt(size)                 __maybenull __in_ecount_opt(size)
#define sal_out_ecount(size)                    __notnull   __out_ecount(size)
#define sal_out_ecount_opt(size)                __maybenull __out_ecount_opt(size)
#define sal_inout_ecount(size)                  __notnull   __inout_ecount(size)
#define sal_inout_ecount_opt(size)              __maybenull __inout_ecount_opt(size)

#define sal_checkReturn                         __checkReturn
#define sal_callback                            __callback
#define sal_format_string                       __format_string

#define sal_field_bcount(size)                  __field_bcount(size)
#define sal_field_bcount_opt(size)              __field_bcount_opt(size)
#define sal_field_bcount_part(size, length)     __field_bcount_part(size, length)
#define sal_field_bcount_full(size)             __field_bcount_full(size)
#define sal_field_ecount(size)                  __field_ecount(size)
#define sal_field_ecount_opt(size)              __field_ecount_opt(size)
#define sal_field_ecount_part(size, length)     __field_ecount_part(size, length)
#define sal_field_ecount_full(size)             __field_ecount_full(size)

#define sal_deref_out_range(op, expr)           __deref_out_range(op, expr)

#else

// TODO: see if sal_checkReturn can be made __attribute__((warn_unused_result))

#define sal_in                      
#define sal_in_opt                  
#define sal_in_z                    
#define sal_in_z_opt                
#define sal_out                     
#define sal_out_opt                 
#define sal_out_z                   
#define sal_out_z_opt               
#define sal_inout                   
#define sal_inout_opt               
#define sal_inout_z                 
#define sal_inout_z_opt             

#define sal_in_bcount(size)         
#define sal_in_bcount_opt(size)     
#define sal_out_bcount(size)        
#define sal_out_bcount_opt(size)    
#define sal_inout_bcount(size)      
#define sal_inout_bcount_opt(size)  
#define sal_in_ecount(size)         
#define sal_in_ecount_opt(size)     
#define sal_out_ecount(size)        
#define sal_out_ecount_opt(size)    
#define sal_inout_ecount(size)      
#define sal_inout_ecount_opt(size)  

#define sal_checkReturn             
#define sal_callback                
#define sal_format_string           

#define sal_field_bcount(size)                  
#define sal_field_bcount_opt(size)              
#define sal_field_bcount_part(size, length)     
#define sal_field_bcount_full(size)             
#define sal_field_ecount(size)                  
#define sal_field_ecount_opt(size)              
#define sal_field_ecount_part(size, length)     
#define sal_field_ecount_full(size)             

#define sal_deref_out_range(op, expr)           

#endif