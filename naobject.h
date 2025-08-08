#ifndef	NAOBJECT_H
#define	NAOBJECT_H

class	CNAObject
{
	int		m_status;
	char	*m_message;
public:
	CNAObject(){
		m_status=0;
		m_message= NULL;
	}
	void	setMessage(int status, char *message)
	{
		m_status= status;
		m_message= message;
	}
	char	*getMessage()
	{
		if(m_status == 0)	return "";
		return m_message;
	}
};

#endif