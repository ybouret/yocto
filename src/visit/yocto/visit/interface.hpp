#ifndef YOCTO_VISIT_INTERFACE_INCLUDED
#define YOCTO_VISIT_INTERFACE_INCLUDED 1

#include "VisItControlInterface_V2.h"
#include "VisItDataInterface_V2.h"
#include "yocto/mpi/mpi.hpp"
#include "yocto/threading/singleton.hpp"
#include "yocto/functor.hpp"
#include "yocto/associative/map.hpp"

namespace yocto
{
    class VisIt : public singleton<VisIt>
    {
    public:
        class Simulation; //!< forward

        const mpi   &MPI;
        
        static VisIt &Start(const string &sim_name,
                            const string &sim_comment,
                            const mpi    &usrMPI,
                            const string *sim_gui  = NULL,
                            const bool    usrTrace = false);

        
        
    private:
        YOCTO_DISABLE_COPY_AND_ASSIGN(VisIt);
        friend class singleton<VisIt>;
        virtual ~VisIt() throw();
        explicit VisIt();

        static const threading::longevity life_time = -10;

    public:
        static const char name[];


        class Simulation
        {
        public:
#define YOCTO_VISIT_SIMULATION_CALLBACK_FUNCT TL2(const string &, const array<string> &)
#define YOCTO_VISIT_SIMULATION_CALLBACK_PROTO const string &cmd, const array<string> &args

            typedef functor<void,YOCTO_VISIT_SIMULATION_CALLBACK_FUNCT> Callback;
            typedef map<string,Callback>                                CallbackDB;

            const mpi &MPI;
            int        runMode;
            int        cycle;
            double     runTime;
            bool       done;
            const int  connected;
            Callback   doNothing;
            CallbackDB callbacks;

            virtual ~Simulation() throw();
            explicit Simulation( const VisIt &visit );

            void loop();
            void step();
            void update() throw(); //!< send visit signal to update



            void addGenericCommand(visit_handle   &md,
                                   const char     *command_name,
                                   const Callback &cb);

            template <typename OBJECT_POINTER,typename METHOD_POINTER> inline
            void addGenericCommand(visit_handle  &md,
                                   const char    *command_name,
                                   OBJECT_POINTER host,
                                   METHOD_POINTER method)
            {
                const Callback cb(host,method);
                addGenericCommand(md,command_name,cb);
            }

            void on_quit(YOCTO_VISIT_SIMULATION_CALLBACK_PROTO) throw();
            void on_step(YOCTO_VISIT_SIMULATION_CALLBACK_PROTO);
            void on_halt(YOCTO_VISIT_SIMULATION_CALLBACK_PROTO) throw();
            void on_run( YOCTO_VISIT_SIMULATION_CALLBACK_PROTO) throw();
            void do_nope(YOCTO_VISIT_SIMULATION_CALLBACK_PROTO) throw();

        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(Simulation);
            virtual void one_step();
            void addGenericCommand(visit_handle   &md,
                                   const char     *command_name);
            



        };
    };
}

#endif
