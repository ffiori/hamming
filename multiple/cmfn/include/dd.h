#ifndef	_DUFFS_DEVICE_H_
#define	_DUFFS_DEVICE_H_

#define	DUFFSDEVICE( duffed, duff_statement )			\
{								\
	int	duffi;						\
								\
	switch( duffi = ( duffed + 7 ) / 8, duffed % 8 )	\
	{							\
		do {    case 0:	duff_statement;			\
	                case 7:	duff_statement;			\
        	        case 6:	duff_statement;			\
                	case 5:	duff_statement;			\
                	case 4:	duff_statement;			\
                	case 3:	duff_statement;			\
			case 2:	duff_statement;			\
			case 1:	duff_statement;			\
								\
	         } while ( --duffi );				\
        }							\
}								

#endif	// _DUFFS_DEVICE_H_

