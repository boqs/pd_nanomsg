#include "m_pd.h"
#include <nanomsg/nn.h>
#include <nanomsg/pubsub.h>

 
static t_class *nanomsg_class;  
 
typedef struct _nanomsg {  
  t_object  x_obj;
  int fd;
  t_outlet *m_out;
  t_clock *poll_timer;
} t_nanomsg;  
 
void grid_tick(void *client) {
  t_nanomsg *n = client;
  double tick_start = clock_getlogicaltime();
  char msg[256];
  int rc = nn_recv (n->fd, msg, sizeof (msg), NN_DONTWAIT);
  if (rc != -1) {
    outlet_anything(n->m_out, gensym(msg), 0, NULL);

  }
  clock_delay(n->poll_timer, 100.0 - clock_gettimesince(tick_start));
}

void *nanomsg_new(t_symbol *s, int argc, t_atom *argv)  
{
  (void) s;
 
  if(argc == 1 && argv->a_type == A_SYMBOL) {
   t_nanomsg *x = (t_nanomsg *)pd_new(nanomsg_class);  
    x->fd = nn_socket (AF_SP, NN_SUB);
    if (x->fd < 0) {
        fprintf (stderr, "nn_socket: %s\n", nn_strerror (nn_errno ()));
        return NULL;
    }
    char addr[256];
    atom_string(argv, addr, sizeof(addr));
    printf("addr: %s\n", addr);
    if (nn_connect (x->fd, addr) < 0) {
        fprintf (stderr, "nn_socket: %s\n", nn_strerror (nn_errno ()));
        nn_close (x->fd);
        return NULL;
    }

    /*  We want all messages, so just subscribe to the empty value. */
    if (nn_setsockopt (x->fd, NN_SUB, NN_SUB_SUBSCRIBE, "", 0) < 0) {
        fprintf (stderr, "nn_setsockopt: %s\n", nn_strerror (nn_errno ()));
        nn_close (x->fd);
        return NULL;
    }
     x->m_out = outlet_new(&x->x_obj, &s_symbol);
     x->poll_timer = clock_new(x, (t_method) grid_tick);
     clock_delay(x->poll_timer, 100.0);

     return (void *)x;  
  }
  return NULL;
}
 
void nanomsg_setup(void) {
  nanomsg_class = class_new(gensym("nanomsg"),  
			    (t_newmethod)nanomsg_new,  
			    0, sizeof(t_nanomsg),  
			    CLASS_DEFAULT,  
			    A_GIMME, 0);
 
  class_sethelpsymbol(nanomsg_class, gensym("help-nanomsg"));  
}
