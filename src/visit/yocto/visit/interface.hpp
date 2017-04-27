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
            class CallbackInfo
            {
            public:
                Callback   cb;
                const bool ui;
                inline  CallbackInfo(const Callback &userCB, const bool userUI) :
                cb(userCB),
                ui(userUI)
                {
                }

                inline ~CallbackInfo() throw()
                {
                }

                inline CallbackInfo(const CallbackInfo &other) :
                cb(other.cb),
                ui(other.ui)
                {
                }
                

            private:
                YOCTO_DISABLE_ASSIGN(CallbackInfo);
            };

            typedef map<string,CallbackInfo> CallbackDB;

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



            void addCommand( const char     *command_name,
                            const Callback  &cb,
                            const bool       ui);

            template <typename OBJECT_POINTER,typename METHOD_POINTER> inline
            void addCommand(const char    *command_name,
                            OBJECT_POINTER host,
                            METHOD_POINTER method,
                            const bool     ui)
            {
                const Callback cb(host,method);
                addCommand(command_name,cb,ui);
            }

            void addGenericCommands(visit_handle &md) const;

            void on_quit(YOCTO_VISIT_SIMULATION_CALLBACK_PROTO) throw();
            void on_step(YOCTO_VISIT_SIMULATION_CALLBACK_PROTO);
            void on_halt(YOCTO_VISIT_SIMULATION_CALLBACK_PROTO) throw();
            void on_run( YOCTO_VISIT_SIMULATION_CALLBACK_PROTO) throw();
            void do_nope(YOCTO_VISIT_SIMULATION_CALLBACK_PROTO) throw();

        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(Simulation);
            virtual void one_step();
            void addGenericCommand(visit_handle   &md,
                                   const char     *command_name) const;
            



        };
    };
}

#endif
