#ifndef	NALINK_H
#define	NALINK_H

typedef	enum {LINKTYPE_SINGLE, LINKTYPE_ARRAY} LinkType;
typedef	enum {LINKSTATUS_DIE, LINKSTATUS_ALIVE} LinkStatus;

class	CNALink: public CNAObject
{
public:
	CNALink			*m_next;
	CNALink			*m_prev;
//	void			*m_element;
	CNAEntry			*m_element;
	LinkType		m_type;	
	LinkStatus		m_status; 
};

#endif;