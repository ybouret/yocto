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

        //! initialize VisIt interface
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

        //! base class for a simulation
        class Simulation
        {
        public:
            //__________________________________________________________________
            //
            // wrapper to handle UI commands and user commands
            //__________________________________________________________________

#define YOCTO_VISIT_SIMULATION_CALLBACK_FUNCT TL2(const string &, const array<string> &)
#define YOCTO_VISIT_SIMULATION_CALLBACK_PROTO const string &cmd, const array<string> &args

            //! Callback definition, command + sequence of arguments
            typedef functor<void,YOCTO_VISIT_SIMULATION_CALLBACK_FUNCT> Callback;

            class CallbackInfo
            {
            public:
                Callback   cb;
                const bool ui;
                inline  CallbackInfo(const Callback &userCB,
                                     const bool      userUI) :
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
            int        runMode;   //!< Stopped/Running
            int        cycle;     //!< counter
            double     runTime;   //!< runTime
            bool       done;      //!< will terminate simulation
            const int  connected; //!< updated during each loop
            Callback   doNothing; //!< a do nothing callback
            CallbackDB callbacks; //!< database of callbacks

            virtual ~Simulation() throw();
            explicit Simulation( const VisIt &visit );

            void loop();
            void update() throw(); //!< send visit signal to update
            void step();           //!< one_step and update


            //! add a command, appearing in UI is true==ui
            void addCommand(const char      *command_name,
                            const Callback  &cb,
                            const bool       ui);

            //! wrapper to add a command
            template <typename OBJECT_POINTER,typename METHOD_POINTER> inline
            void addCommand(const char    *command_name,
                            OBJECT_POINTER host,
                            METHOD_POINTER method,
                            const bool     ui)
            {
                const Callback cb(host,method);
                addCommand(command_name,cb,ui);
            }


            //! declare commands for UI
            void addGenericCommands(visit_handle &md) const;

            void on_quit(YOCTO_VISIT_SIMULATION_CALLBACK_PROTO) throw(); //!< done=true
            void on_step(YOCTO_VISIT_SIMULATION_CALLBACK_PROTO);         //!< step(s)
            void on_halt(YOCTO_VISIT_SIMULATION_CALLBACK_PROTO) throw(); //!< runMode = STOPPED
            void on_run( YOCTO_VISIT_SIMULATION_CALLBACK_PROTO) throw(); //!< runMode = RUNNING
            void do_nope(YOCTO_VISIT_SIMULATION_CALLBACK_PROTO) throw(); //!< do nothing...

            virtual void setMetaData(visit_handle &md);

        private:

            YOCTO_DISABLE_COPY_AND_ASSIGN(Simulation);
            void addGenericCommand(visit_handle   &md,
                                   const char     *command_name) const;

            virtual void one_step();

        };
    };
}

#endif
